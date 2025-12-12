#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../helpers/parser.h"

#define MAX_MACHINES 256
#define MAX_COUNTERS 64
#define MAX_BUTTONS 64
#define MAX_BUTTON_COLS 64
#define HASH_SIZE 65536

struct MemoEntry {
    long long *targets;
    int len;
    long long result;
    struct MemoEntry *next;
};

struct MemoEntry *memo_table[HASH_SIZE];

unsigned int hash_targets(long long *targets, int len) {
    unsigned int hash = 5381;
    for (int i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + (unsigned int)(targets[i] & 0xFFFFFFFF);
        hash = ((hash << 5) + hash) + (unsigned int)(targets[i] >> 32);
    }
    return hash & (HASH_SIZE - 1);
}

long long get_memo(long long *targets, int len) {
    unsigned int h = hash_targets(targets, len);
    struct MemoEntry *e = memo_table[h];
    while (e) {
        if (e->len == len) {
            bool match = true;
            for (int i = 0; i < len; i++) {
                if (e->targets[i] != targets[i]) {
                    match = false;
                    break;
                }
            }
            if (match) return e->result;
        }
        e = e->next;
    }
    return -2; // -2 indicates not found (-1 is used for 'impossible')
}

void add_memo(long long *targets, int len, long long result) {
    unsigned int h = hash_targets(targets, len);
    struct MemoEntry *n = malloc(sizeof(struct MemoEntry));
    n->targets = malloc(sizeof(long long) * len);
    memcpy(n->targets, targets, sizeof(long long) * len);
    n->len = len;
    n->result = result;
    n->next = memo_table[h];
    memo_table[h] = n;
}

void clear_memo() {
    for (int i = 0; i < HASH_SIZE; i++) {
        struct MemoEntry *e = memo_table[i];
        while (e) {
            struct MemoEntry *tmp = e;
            e = e->next;
            free(tmp->targets);
            free(tmp);
        }
        memo_table[i] = NULL;
    }
}

// --- Gaussian Elimination GF(2) ---

// Solve Ax = b over GF(2)
// A is passed as button_masks (columns of the matrix, actually).
// We need to find all subsets of buttons that XOR to target_mask.
// Returns a list of solution masks (integers where bit k set means button k is pressed).
// Since number of solutions can be 2^(free_vars), we assume it's manageable (Part 1 logic).
// If num_buttons > 64, this fails (using unsigned long long for mask).
int get_xor_solutions(int num_buttons, int num_counters, 
                      int buttons_impact[MAX_BUTTONS][MAX_BUTTON_COLS], 
                      unsigned long long target_mask, 
                      unsigned long long *solutions_out) {
    
    unsigned long long mat[MAX_COUNTERS];
    unsigned long long rhs = target_mask;
    
    for (int r = 0; r < num_counters; r++) {
        mat[r] = 0;
        for (int b = 0; b < num_buttons; b++) {
            bool affects = false;
            for (int k = 0; k < MAX_BUTTON_COLS; k++) {
                if (buttons_impact[b][k] == -1) break;
                if (buttons_impact[b][k] == r) {
                    affects = true;
                    break;
                }
            }
            if (affects) {
                mat[r] |= (1ULL << b);
            }
        }
    }

    int pivot_col_to_row[MAX_BUTTONS];
    for (int i = 0; i < MAX_BUTTONS; i++) pivot_col_to_row[i] = -1;
    
    int rank = 0;
    int pivot_row = 0;

    // Gaussian Elimination
    for (int col = 0; col < num_buttons && pivot_row < num_counters; col++) {
        int best = -1;
        for (int r = pivot_row; r < num_counters; r++) {
            if ((mat[r] >> col) & 1) {
                best = r;
                break;
            }
        }

        if (best == -1) continue;

        if (best != pivot_row) {
            unsigned long long t = mat[pivot_row]; mat[pivot_row] = mat[best]; mat[best] = t;
            int b1 = (rhs >> pivot_row) & 1;
            int b2 = (rhs >> best) & 1;
            if (b1 != b2) {
                rhs ^= (1ULL << pivot_row);
                rhs ^= (1ULL << best);
            }
        }

        pivot_col_to_row[col] = pivot_row;

        for (int r = 0; r < num_counters; r++) {
            if (r != pivot_row && ((mat[r] >> col) & 1)) {
                mat[r] ^= mat[pivot_row];
                if ((rhs >> pivot_row) & 1) {
                    rhs ^= (1ULL << r);
                }
            }
        }
        
        pivot_row++;
        rank++;
    }

    for (int r = pivot_row; r < num_counters; r++) {
        if ((rhs >> r) & 1) return 0;
    }

    int free_vars[MAX_BUTTONS];
    int num_free = 0;
    for (int b = 0; b < num_buttons; b++) {
        if (pivot_col_to_row[b] == -1) {
            free_vars[num_free++] = b;
        }
    }

    int num_solutions = 0;
    int max_solutions = 1 << num_free; // 2^num_free

    for (int i = 0; i < max_solutions; i++) {
        unsigned long long x = 0;

        for (int f = 0; f < num_free; f++) {
            if ((i >> f) & 1) {
                x |= (1ULL << free_vars[f]);
            }
        }

        for (int b = num_buttons - 1; b >= 0; b--) {
            int r = pivot_col_to_row[b];
            if (r != -1) {
                // Equation: Pivot + Sum(Coeff * Knowns) = RHS
                // Pivot = RHS - Sum(...)  (Sub is XOR in GF2)
                int val = (rhs >> r) & 1;
                // XOR with other cols in this row
                unsigned long long row_mask = mat[r];
                // We need to XOR bits corresponding to variables > b (already set)
                // Actually, since we eliminated above and below, mat[r] only has the pivot bit 'b' 
                // and bits for free variables to the right? 
                // Wait, Gauss-Jordan (eliminating above and below) simplifies this.
                // My elimination loop `for (int r = 0; r < num_counters; r++)` does Gauss-Jordan.
                // So mat[r] contains the pivot bit and potentially bits for *free* variables.
                // It does NOT contain bits for other pivot variables.
                
                // So PivotVal = RHS[r] ^ (RowBits & CurrentX)
                // (RowBits & CurrentX) accounts for the free variables contribution
                if (__builtin_parityll(row_mask & x) % 2 != 0) {
                    val ^= 1;
                }
                
                if (val) x |= (1ULL << b);
            }
        }
        solutions_out[num_solutions++] = x;
    }

    return num_solutions;
}

long long solve_recursive(long long *targets, int num_counters, int num_buttons, 
                   int buttons_impact[MAX_BUTTONS][MAX_BUTTON_COLS]) {
    
    long long cached = get_memo(targets, num_counters);
    if (cached != -2) return cached;

    bool all_zero = true;
    for (int i = 0; i < num_counters; i++) {
        if (targets[i] != 0) {
            all_zero = false;
            break;
        }
    }
    if (all_zero) return 0;

    unsigned long long parity_mask = 0;
    for (int i = 0; i < num_counters; i++) {
        if (targets[i] % 2 != 0) {
            parity_mask |= (1ULL << i);
        }
    }

    unsigned long long solutions[4096]; 
    int num_sols = get_xor_solutions(num_buttons, num_counters, buttons_impact, parity_mask, solutions);

    if (num_sols == 0) {
        add_memo(targets, num_counters, -1);
        return -1; // Impossible
    }

    long long best_presses = -1;

    long long next_targets[MAX_COUNTERS];

    for (int s = 0; s < num_sols; s++) {
        unsigned long long btn_mask = solutions[s];
        
        bool valid = true;
        int presses_count = 0;
        
        for(int i=0; i<num_counters; i++) next_targets[i] = targets[i];

        for (int b = 0; b < num_buttons; b++) {
            if ((btn_mask >> b) & 1) {
                presses_count++;
                for (int k = 0; k < MAX_BUTTON_COLS; k++) {
                    int c_idx = buttons_impact[b][k];
                    if (c_idx == -1) break;
                    next_targets[c_idx]--;
                }
            }
        }

        for (int i = 0; i < num_counters; i++) {
            if (next_targets[i] < 0 || next_targets[i] % 2 != 0) {
                valid = false;
                break;
            }
            next_targets[i] /= 2;
        }

        if (valid) {
            long long sub_res = solve_recursive(next_targets, num_counters, num_buttons, buttons_impact);
            if (sub_res != -1) {
                // Cost = CurrentPresses + 2 * RecursiveCost
                long long total = presses_count + 2 * sub_res;
                if (best_presses == -1 || total < best_presses) {
                    best_presses = total;
                }
            }
        }
    }

    add_memo(targets, num_counters, best_presses);
    return best_presses;
}

int main() {
    const char *filename = "big-input.txt";
    const char *filemode = "r";
    struct parser_context pctx;
    init_parser(&pctx, filename, filemode);

    const char *delimeters = "\n";
    int lines = 0;
    
    long long joltages[MAX_MACHINES][MAX_COUNTERS];
    int joltages_len[MAX_MACHINES];
    int buttons_choices[MAX_MACHINES][MAX_BUTTONS][MAX_BUTTON_COLS];
    memset(buttons_choices, -1, sizeof(buttons_choices));
    int buttons_line_groups[MAX_MACHINES];

    int j_ptr = 0;
    int buttons_group = 0;
    int buttons_cols = 0;
    
    while (next_token(&pctx, delimeters, false)) {
        size_t token_len = strlen(pctx.token);
        int token_ptr = 0;
        bool save_joltage = false;
        bool save_button = false;
        long long curr_val = 0;
        bool reading_num = false;
        
        while (token_ptr < token_len) {
            char c = pctx.token[token_ptr];
            if (c == '{') { save_joltage = true; reading_num = false; }
            else if (c == '}') {
                save_joltage = false;
                if (reading_num) joltages[lines][j_ptr++] = curr_val;
                curr_val = 0; reading_num = false;
            } else if (save_joltage && isdigit(c)) {
                reading_num = true; curr_val = (curr_val * 10) + (c - '0');
            } else if (c == ',' && save_joltage) {
                if (reading_num) joltages[lines][j_ptr++] = curr_val;
                curr_val = 0; reading_num = false;
            }
            else if (c == '(') { save_button = true; reading_num = false; buttons_cols = 0; }
            else if (c == ')') {
                save_button = false;
                if (reading_num) buttons_choices[lines][buttons_group][buttons_cols++] = (int)curr_val;
                curr_val = 0; reading_num = false;
                buttons_group++;
                buttons_cols = 0;
            } else if (save_button && isdigit(c)) {
                reading_num = true; curr_val = (curr_val * 10) + (c - '0');
            } else if (c == ',') {
                if (reading_num) buttons_choices[lines][buttons_group][buttons_cols++] = (int)curr_val;
                curr_val = 0;
            }
            token_ptr++;
        }
        joltages_len[lines] = j_ptr; j_ptr = 0;
        buttons_line_groups[lines] = buttons_group; buttons_group = 0;
        lines++;
    }

    long long grand_total = 0;

    for (int i = 0; i < lines; i++) {
        clear_memo();
        
        if (buttons_line_groups[i] > 60) {
            printf("ERROR: Too many buttons for 64-bit mask implementation!\n");
            return 1;
        }

        long long res = solve_recursive(joltages[i], joltages_len[i], buttons_line_groups[i], buttons_choices[i]);
        
        if (res != -1) {
            grand_total += res;
            // printf("Machine %d: %lld\n", i, res);
        } else {
            // printf("Machine %d: IMPOSSIBLE\n", i);
        }
    }

    printf("INFO: Answer [%lld]\n", grand_total);
    close_parser(&pctx);
    return 0;
}
