#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX 255

char * getCurrentTime(){
	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	return asctime(timeinfo);
}

struct item{
	int id;
	char name[MAX];
	int stock;
	double buyPrice;
	double sellPrice;
} typedef Item;

struct transaction{
	int id;
	int itemId;
	int quantity;
	double totalPrice;
	char createdAt[MAX];
} typedef Transaction;

struct itemNode{
	Item value;
	struct itemNode *next;
	struct itemNode *prev;
} typedef ItemNode;

struct transactionNode{
	Transaction value;
	struct transactionNode *next;
	struct transactionNode *prev;
} typedef TransactionNode;

ItemNode *itemList;
TransactionNode *purchaseList;
TransactionNode *salesList;

int depletedItem(Item item){
	return item.stock <= 0;
}

int showAllItem(Item item){
	return true;
}

int getTotalItem(ItemNode* n){
	int size=0;
    while (n != NULL) {
        size++;
        n = n->next;
    }
    return size;
}

int getTotalTransaction(TransactionNode* n){
	int size=0;
    while (n != NULL) {
        size++;
        n = n->next;
    }
    return size;
}

void addItem(ItemNode **head, Item value, int index){
	ItemNode * temp1 = (ItemNode*)malloc(sizeof(ItemNode));
	temp1->value = value;
	temp1->next = NULL;
	temp1->prev = NULL;
	if(index==1){
		temp1->next = *head;
		*head = temp1;
		return;
	}
	ItemNode * temp2 = *head;
	for(int i=0; i<index-2;i++){
		temp2 = temp2->next;
	}
	temp1->next = temp2->next;
	temp1->prev = temp2;
	temp2->next = temp1;
}

void addTransaction(TransactionNode **head, Transaction value, int index){
	TransactionNode * temp1 = (TransactionNode*)malloc(sizeof(TransactionNode));
	temp1->value = value;
	temp1->next = NULL;
	temp1->prev = NULL;
	if(index==1){
		temp1->next = *head;
		*head = temp1;
		return;
	}
	TransactionNode * temp2 = *head;
	for(int i=0; i<index-2;i++){
		temp2 = temp2->next;
	}
	temp1->next = temp2->next;
	temp1->prev = temp2;
	temp2->next = temp1;
}

Item findByName(ItemNode *list, char *name){
	while (list != NULL) {
		int compare = strcmp((list->value).name, name);
        if(compare == 0){
        	return list->value;
		}
        list = list->next;
    }
    Item nulled;
    nulled.id = -1;
    return nulled;
}

Item findById(ItemNode *list, int id){
	while (list != NULL) {
        if(id == (list->value).id){
        	return list->value;
		}
        list = list->next;
    }
    Item nulled;
    nulled.id = -1;
    return nulled;
}

void updateItem(ItemNode *list, char *name, Item newItem){
	int found = 0;
	while (list != NULL) {
		int compare = strcmp((list->value).name, name);
        if(compare == 0){
        	found = 1;
        	break;
		}
        list = list->next;
    }
    if(found){
    	list->value = newItem;
	}
    
}

void purchaseItem(){
	char name[MAX];
	Transaction newTrx;
	printf("--Purchase Item--\n");
	printf("Please input item name: ");
	scanf("%s",name);
	Item existing = findByName(itemList, name);
	if(existing.id == -1){
		printf("\nItem %s is not existed, thus save as a new item...\n", name);
		existing.id = getTotalItem(itemList)+1;
		strcpy(existing.name, name);
		existing.stock = 0;
		existing.buyPrice = 0.0;
		existing.sellPrice = 0.0;
		addItem(&itemList, existing, 1);
	}
	newTrx.itemId = existing.id;
	printf("Quantity: \n");
	scanf("%d",&newTrx.quantity);
	existing.stock += newTrx.quantity;
	printf("Price per Item: \n");
	double itemPrice;
	scanf("%lf",&itemPrice);
	existing.buyPrice = itemPrice;
	newTrx.totalPrice = itemPrice*newTrx.quantity;
	
	printf("Selling Price per Item: \n");
	double itemSellPrice;
	scanf("%lf",&itemSellPrice);
	existing.sellPrice = itemSellPrice;
	
	strcpy(newTrx.createdAt, getCurrentTime());
	
	addTransaction(&purchaseList,newTrx,1);
	updateItem(itemList, name, existing);
}

void sellItem(){
	char name[MAX];
	Transaction newTrx;
	printf("--Purchase Item--\n");
	printf("Please input item name: ");
	scanf("%s",name);
	Item existing = findByName(itemList, name);
	if(existing.id == -1){
		printf("\nItem %s is not existed...\n", name);
		return;
	}
	int qty;
	printf("Quantity: \n");
	scanf("%d",&qty);
	if(existing.stock < qty){
		printf("\nInsufficient stock. Item %s remaining stock is %d\n", existing.name,existing.stock);
		return;
	}
	existing.stock -= qty;
	newTrx.itemId = existing.id;
	newTrx.quantity = qty;
	newTrx.totalPrice = existing.sellPrice*newTrx.quantity;
	
	strcpy(newTrx.createdAt, getCurrentTime());
	
	addTransaction(&salesList,newTrx,1);
	updateItem(itemList, name, existing);
}

void showTransaction(TransactionNode *list, char *title){
	printf("--%s--\n",title);
	int no=1;
	while (list != NULL) {
		Transaction transaction = list->value;
		Item item = findById(itemList, transaction.itemId);
		printf("------------------------\n");
		printf("Transaction Date: %s \n",transaction.createdAt);
		printf("Item Name: %s \n",item.name);
		printf("Quantity: %d \n",transaction.quantity);
		printf("Total Price: %lf \n",transaction.totalPrice);
		printf("------------------------\n\n");
        list = list->next;
        no++;
    }
}

void showItemStock(ItemNode *list, int (*criteria)(Item), char *title){
	printf("--%s--\n",title);
	printf("No.\t ItemName\t Stock\t Purchase Price\t \tSell Price\n");
	int no=1;
	while (list != NULL) {
		Item item = list->value;
		if(criteria(item)){
			printf("%d.\t %s\t %d\t %lf\t %lf\n",no,item.name,item.stock,item.buyPrice,item.sellPrice);
	        no++;
		}
		list = list->next;
    }
    printf("------------------------\n\n");
}

int mainMenu(){
	int action;
	printf("Welcome to BUDI ATK Store\n");
	printf("1. See Item Stock\n");
	printf("2. See depleted Item\n");
	printf("3. Add Item Purchase\n");
	printf("4. Add Item Sell\n");
	printf("5. See Purchasing Item\n");
	printf("6. See Selling Item\n");
	printf("7. Find Item By Name\n");
	printf("8. Exit\n");
	scanf("%d",&action);
	return action;
}

int main(void){
	itemList = NULL;
	purchaseList = NULL;
	salesList = NULL;
	
	int action = 0;
	while(action!=8){
		action = mainMenu();
		switch(action){
			case 1:{
				showItemStock(itemList, showAllItem, "All Item Stock");
				break;
			}
			case 2:{
				showItemStock(itemList, depletedItem, "All Depleted Item");
				break;
			}
			case 3:{
				purchaseItem();
				break;
			}
			case 4:{
				sellItem();
				break;
			}
			case 5:{
				showTransaction(purchaseList,"All Purchasing Item"); 
				break;
			}
			case 6:{
				showTransaction(salesList,"All Selling Item"); 
				break;
			}
			case 7:{
				char name[MAX];
				scanf("%s",name);
				Item item = findByName(itemList, name);
				printf("%s\t %s\t %d\t %lf\n",item.name,item.stock,item.buyPrice,item.sellPrice);
				break;
			}
		}
	}

	return 0;
}


