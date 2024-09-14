#define BASE_C_IMPLEMENTATION 1
#include "base.h"
#include <stdio.h>

struct Person {
	cstring name;
	int age;

	List_Node list;
};

static struct Person people[] = {
	{.name = "alice",    .age = 19, .list = {0}},
	{.name = "bob",      .age = 21, .list = {0}},
	{.name = "tim",      .age = 43, .list = {0}},
	{.name = "theo",     .age = 8,  .list = {0}},
	{.name = "margaret", .age = 83, .list = {0}},
};

int main(){
	Bytes b = file_read_all(str_from("main.c"), heap_allocator());
	printf("%.*s", FmtString(b));

	struct Person_List {
		List_Node head;
	};

	struct Person_List people_list = {0};
	list_init(&people_list.head);

	list_add(&people_list.head, &people[0].list);
	list_add(&people_list.head, &people[1].list);
	list_add(&people_list.head, &people[2].list);
	list_add(&people_list.head, &people[3].list);
	list_add(&people_list.head, &people[4].list);

	list_foreach(node, people_list.head){
		struct Person* p = list_entry(node, struct Person, list);
		printf("%s %d\n", p->name, p->age);
	}

	printf("REVERSED\n");
	list_foreach_reversed(node, people_list.head){
		struct Person* p = list_entry(node, struct Person, list);
		printf("%s %d\n", p->name, p->age);
	}

	mem_free(heap_allocator(), b.data);
	return 0;
}
