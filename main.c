#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 50
#define PRICE_HISTORY 5

// -------------------- STRUCTURES --------------------

typedef struct Queue {
    float prices[PRICE_HISTORY];
    int front, rear, count;
} Queue;

typedef struct Stock {
    char name[20];
    float buyPrice;
    float currentPrice;
    int quantity;
    Queue history;
    struct Stock *next;   // for linked list
} Stock;

typedef struct BST {
    char name[20];
    struct BST *left, *right;
} BST;

// -------------------- GLOBALS --------------------

Stock* hashTable[SIZE];
Stock* transactionHead = NULL;
BST* bstRoot = NULL;

// -------------------- HASH FUNCTION --------------------

int hash(char *name) {
    int sum = 0;
    for (int i = 0; name[i]; i++)
        sum += name[i];
    return sum % SIZE;
}

// -------------------- QUEUE FUNCTIONS --------------------

void initQueue(Queue *q) {
    q->front = q->rear = q->count = 0;
}

void enqueue(Queue *q, float price) {
    if (q->count == PRICE_HISTORY) {
        q->front = (q->front + 1) % PRICE_HISTORY;
        q->count--;
    }
    q->prices[q->rear] = price;
    q->rear = (q->rear + 1) % PRICE_HISTORY;
    q->count++;
}

// -------------------- BST FUNCTIONS --------------------

BST* insertBST(BST* root, char *name) {
    if (!root) {
        BST* node = (BST*)malloc(sizeof(BST));
        strcpy(node->name, name);
        node->left = node->right = NULL;
        return node;
    }
    if (strcmp(name, root->name) < 0)
        root->left = insertBST(root->left, name);
    else
        root->right = insertBST(root->right, name);
    return root;
}

void inorderBST(BST* root) {
    if (root) {
        inorderBST(root->left);
        printf("%s\n", root->name);
        inorderBST(root->right);
    }
}

// -------------------- CORE FUNCTIONS --------------------

void addStock() {
    Stock* s = (Stock*)malloc(sizeof(Stock));

    printf("Enter Stock Name: ");
    scanf("%s", s->name);
    printf("Buy Price: ");
    scanf("%f", &s->buyPrice);
    printf("Current Price: ");
    scanf("%f", &s->currentPrice);
    printf("Quantity: ");
    scanf("%d", &s->quantity);

    initQueue(&s->history);
    enqueue(&s->history, s->currentPrice);

    int index = hash(s->name);
    s->next = hashTable[index];
    hashTable[index] = s;

    // Transaction history
    s->next = transactionHead;
    transactionHead = s;

    bstRoot = insertBST(bstRoot, s->name);

    printf("Stock Added Successfully!\n");
}

void updatePrice() {
    char name[20];
    float price;
    printf("Enter Stock Name: ");
    scanf("%s", name);
    printf("New Price: ");
    scanf("%f", &price);

    int index = hash(name);
    Stock* temp = hashTable[index];

    while (temp) {
        if (strcmp(temp->name, name) == 0) {
            temp->currentPrice = price;
            enqueue(&temp->history, price);
            printf("Price Updated!\n");
            return;
        }
        temp = temp->next;
    }
    printf("Stock Not Found!\n");
}

void portfolioSummary() {
    float investment = 0, currentValue = 0;
    Stock* temp;

    for (int i = 0; i < SIZE; i++) {
        temp = hashTable[i];
        while (temp) {
            investment += temp->buyPrice * temp->quantity;
            currentValue += temp->currentPrice * temp->quantity;
            temp = temp->next;
        }
    }

    printf("\nTotal Investment: %.2f", investment);
    printf("\nCurrent Value: %.2f", currentValue);
    printf("\nNet Profit/Loss: %.2f\n", currentValue - investment);
}

void topGainersLosers() {
    Stock *max = NULL, *min = NULL, *temp;

    for (int i = 0; i < SIZE; i++) {
        temp = hashTable[i];
        while (temp) {
            if (!max || temp->currentPrice > max->currentPrice)
                max = temp;
            if (!min || temp->currentPrice < min->currentPrice)
                min = temp;
            temp = temp->next;
        }
    }

    if (max)
        printf("\nTop Gainer: %s (%.2f)", max->name, max->currentPrice);
    if (min)
        printf("\nTop Loser: %s (%.2f)\n", min->name, min->currentPrice);
}

void displaySortedStocks() {
    printf("\nStocks in Sorted Order:\n");
    inorderBST(bstRoot);
}

// -------------------- MENU --------------------

int main() {
    int choice;

    while (1) {
        printf("\n--- Dynamic Stock Portfolio Analyzer ---\n");
        printf("1. Add Stock\n");
        printf("2. Update Stock Price\n");
        printf("3. Portfolio Summary\n");
        printf("4. Top Gainers & Losers\n");
        printf("5. Display Sorted Stocks\n");
        printf("6. Exit\n");
        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addStock(); break;
            case 2: updatePrice(); break;
            case 3: portfolioSummary(); break;
            case 4: topGainersLosers(); break;
            case 5: displaySortedStocks(); break;
            case 6: exit(0);
            default: printf("Invalid Choice!\n");
        }
    }
    return 0;
}
