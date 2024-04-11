#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void merge_sort(int i, int j, pair pair_example[], pair temp[]);
bool has_cycle(int cycle_start, int cycle_end);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Outer loop runs for each voter
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        // This is created to store 1 voter's preferences
        int ranks[candidate_count];

        // This loop asks for voter to rank candidates
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name,
                      ranks)) // vote is to check validity of vote and records rank of candidate
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs()
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    pair temp[pair_count]; // temp array of pairs
    merge_sort(0, pair_count - 1, pairs, temp);

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (!has_cycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        int notlost_count = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true) // goto next column
            {
                break;
            }
            if (locked[j][i] == false) // chk all rows (i has not lost to j)
            {
                notlost_count++;
                if (notlost_count == candidate_count - 1) // not lost to anyone (except himself)
                {
                    printf("%s\n", candidates[i]);
                    break;
                }
            }
        }
    }
    return;
}

void merge_sort(int i, int j, pair pair_example[], pair temp[])
{
    if (j <= i)
    {
        return;
    }
    int mid = (i + j) / 2;
    merge_sort(i, mid, pairs, temp); // recursion of dividing
    merge_sort(mid + 1, j, pairs, temp);
    int left_pointer = i;
    int right_pointer = mid + 1;
    for (int k = i; k < j + 1; k++)

    {
        int left_winner = pair_example[left_pointer].winner;
        int left_loser = pair_example[left_pointer].loser;
        int right_winner = pair_example[right_pointer].winner;
        int right_loser = pair_example[right_pointer].loser;
        if (left_pointer == mid + 1)
        {
            temp[k] = pair_example[right_pointer];
            right_pointer++;
        }
        else if (right_pointer == j + 1)
        {
            temp[k] = pair_example[left_pointer];
            left_pointer++;
        }
        else if (preferences[left_winner][left_loser] > preferences[right_winner][right_loser])
        {
            temp[k] = pair_example[left_pointer];
            left_pointer++;
        }
        else
        {
            temp[k] = pair_example[right_pointer];
            right_pointer++;
        }
    }
    for (int k = i; k < j + 1; k++)
    {
        pair_example[k] = temp[k];
    }
}

bool has_cycle(int cycle_start, int cycle_end)
{
    if (locked[cycle_end][cycle_start]) // base case i.e for (a,b) we have (b,a)
    {
        return true;
    }
    for (int i = 0; i < candidate_count; i++) // 2nd case
    {
        if (locked[cycle_end][i] == true && has_cycle(cycle_start, i)) // create a cycle
        {
            return true;
        }
    }
    return false;
}
