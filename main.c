#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 50
#define PRICE_HISTORY 5
#define MAX_STOCKS 100

// STRUCTURES

typedef struct Queue {
    float prices[PRICE_HISTORY];
    int front, rear, count;
} Queue;

struct Stock;

typedef struct BST {
    struct Stock *stock;
    struct BST *left, *right;
} BST;

typedef struct Stock {
    char name[20];
    float buyPrice;
    float currentPrice;
    int quantity;
    Queue history;
    struct Stock *next;
} Stock;

// GLOBALS 

Stock* hashTable[SIZE];
BST* bstRoot = NULL;

Stock* maxHeap[MAX_STOCKS];
Stock* minHeap[MAX_STOCKS];
int maxSize = 0, minSize = 0;

// HASH FUNCTION 

int hash(char *name) {
    int sum = 0;
    for (int i = 0; name[i]; i++)
        sum += name[i];
    return sum % SIZE;
}

// QUEUE FUNCTIONS

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

// BST FUNCTIONS

BST* insertBST(BST* root, Stock *s) {
    if (!root) {
        BST* node = (BST*)malloc(sizeof(BST));
        node->stock = s;
        node->left = node->right = NULL;
        return node;
    }

    if (strcmp(s->name, root->stock->name) < 0)
        root->left = insertBST(root->left, s);
    else
        root->right = insertBST(root->right, s);

    return root;
}

void inorderBST(BST* root) {
    if (root) {
        inorderBST(root->left);

        printf("%s  Buy: %.2f  Current: %.2f  Qty: %d\n",
               root->stock->name,
               root->stock->buyPrice,
               root->stock->currentPrice,
               root->stock->quantity);

        inorderBST(root->right);
    }
}

// HEAP HELPERS

void swap(Stock** a, Stock** b) {
    Stock* temp = *a;
    *a = *b;
    *b = temp;
}

// MAX HEAP (Top gainer)

void heapifyUpMax(int i) {
    while (i && maxHeap[(i - 1)/2]->currentPrice < maxHeap[i]->currentPrice) {
        swap(&maxHeap[i], &maxHeap[(i - 1)/2]);
        i = (i - 1)/2;
    }
}

void heapifyDownMax(int i) {
    int largest = i, left = 2*i + 1, right = 2*i + 2;

    if (left < maxSize && maxHeap[left]->currentPrice > maxHeap[largest]->currentPrice)
        largest = left;
    if (right < maxSize && maxHeap[right]->currentPrice > maxHeap[largest]->currentPrice)
        largest = right;

    if (largest != i) {
        swap(&maxHeap[i], &maxHeap[largest]);
        heapifyDownMax(largest);
    }
}

void insertMaxHeap(Stock* s) {
    maxHeap[maxSize] = s;
    heapifyUpMax(maxSize);
    maxSize++;
}

// MIN HEAP (Top loser)

void heapifyUpMin(int i) {
    while (i && minHeap[(i - 1)/2]->currentPrice > minHeap[i]->currentPrice) {
        swap(&minHeap[i], &minHeap[(i - 1)/2]);
        i = (i - 1)/2;
    }
}

void heapifyDownMin(int i) {
    int smallest = i, left = 2*i + 1, right = 2*i + 2;

    if (left < minSize && minHeap[left]->currentPrice < minHeap[smallest]->currentPrice)
        smallest = left;
    if (right < minSize && minHeap[right]->currentPrice < minHeap[smallest]->currentPrice)
        smallest = right;

    if (smallest != i) {
        swap(&minHeap[i], &minHeap[smallest]);
        heapifyDownMin(smallest);
    }
}

void insertMinHeap(Stock* s) {
    minHeap[minSize] = s;
    heapifyUpMin(minSize);
    minSize++;
}

// CORE FUNCTIONS

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

    bstRoot = insertBST(bstRoot, s);

    insertMaxHeap(s);
    insertMinHeap(s);

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

            heapifyDownMax(0);
            heapifyDownMin(0);

            printf("Price Updated!\n");
            return;
        }
        temp = temp->next;
    }
    printf("Stock Not Found!\n");
}

void portfolioSummary() {
    float investment = 0, currentValue = 0;

    for (int i = 0; i < SIZE; i++) {
        Stock* temp = hashTable[i];
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
    if (maxSize == 0) {
        printf("No stocks available\n");
        return;
    }

    printf("\nTop Gainer: %s (%.2f)", maxHeap[0]->name, maxHeap[0]->currentPrice);
    printf("\nTop Loser: %s (%.2f)\n", minHeap[0]->name, minHeap[0]->currentPrice);
}

void displaySortedStocks() {
    printf("\nStocks in Sorted Order:\n");
    inorderBST(bstRoot);
}

// MENU

int main() {
    int choice;

    while (1) {
        printf("\n--- Dynamic Stock Portfolio Analyzer (Final Version) ---\n");
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
}
