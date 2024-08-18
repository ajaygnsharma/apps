#include "list_unit_test.h"

int unit_test_build_123(){
	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_init.build       = build_one_two_three;
	list_operation.print  = print_list;
	list_operation.length = length_list;

	node_t *t = NULL;

	t = (*list_init.build)();
	(*list_operation.print)(t);
	printf("length=%d\n",(*list_operation.length)(t));

	return 0;
}

/**
 * Page 14
 * push test. Its not the same as the function in the article and I have
 * added void pointers to try something here.
 * section 2.1 */
void unit_test_push_1(){
	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_init.build       = build_one_two_three;
	list_init.push        = push_list;

	list_operation.print  = print_list;
	list_operation.length = length_list;

	//This works since head is init to NULL. Length comes as 0
	node_t *t = NULL;
#if(NULL_LIST_LENGTH_TEST)
	//Not initiliazing the head to NULL gives segmentation fault.
	//node_t *t;
#endif

	// test length of an empty string
	printf("length=%d\n",(*list_operation.length)(t));
	t = (*list_init.build)();
	(*list_operation.print)(t);
	printf("length=%d\n",(*list_operation.length)(t));

	int *data;
	data = malloc(sizeof(int));
	*data = 10;
	(*list_init.push)(&t,(void *)*data);
	*data = 13;
	(*list_init.push)(&t,(void *)13);

	(*list_operation.print)(t);
}

/**
 * Page 14. Section 2.2
 * push_test2
 * push with empty head node. Now this is the same as the text book */
void unit_test_push_2(void){
	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_init.build       = build_one_two_three;
	list_init.push        = push_list;

	list_operation.print  = print_list;
	list_operation.length = length_list;

	node_t *t = NULL;

	(*list_init.push)(&t,(void *)1);
	(*list_init.push)(&t,(void *)2);
	(*list_init.push)(&t,(void *)3);

	(*list_operation.print)(t);

	list_operation.length = length_list_using_for_loop;
	printf("length=%d\n",(*list_operation.length)(t));
}

/**
 * Page 18
 * section 2.4
 * change_caller()
 * make list head ptr to NULL
 * */
void unit_test_change_caller(){
	node_t *h1;
	node_t *h2;

	struct list_init_s      list_init;

	list_init.null       = change_to_null;

	(*list_init.null)(&h1);
	(*list_init.null)(&h2);

	//head1 and head2 are NULL at this point
}


/**
 * Page 18.
 * add_at_head()
 *
 * Very easy way to build a list. by adding elements to the head.
 * This is kind of like building a stack. Push in data. Printing it will
 * result in reverse order. Push as in pushing in a stack.
 * 1,2,3 pushed give 3,2,1.
 * section 2.5
 * TODO: Memory drawing.
 * */
node_t *unit_test_add_at_head(){
	node_t *h = NULL;
	int i;

	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_operation.print  = print_list;
	list_init.push        = push_list;

	for(i=1;i<6;i++){
		(*list_init.push)(&h,(void *)i);
	}


	(*list_operation.print)(h);

	return h;
}

/**
 * Page 19:
 * build_with_special_case().
 * build list naturally. 1,2,3... gives or builds 1,2,3 in the list
 * First node is created normally and after that others are added to the
 * tail pointer.
 * TODO: Memory drawing.
 * */
node_t *unit_test_build_with_special_case(){
	node_t *h = NULL;
	node_t *t;
	int i;

	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_operation.print  = print_list;
	list_init.push        = push_list;

	/* first push needs to be on head ptr */
	(*list_init.push)(&h,(void *)1);
	t = h;

	/* start with 2 since we are pushing in 1 first then 2....*/
	for(i=2; i<6;i++){
		(*list_init.push)(&t->link,(void *)i);
		t=t->link;
	}

	(*list_operation.print)(h);


	return h;
}



/**
 * Page 20:
 * Instead of pushing a head node first using push and then using tail.
 * This method uses a dummy node which is allocated on the stack.
 * Everything is added to it as a tail.
 * TODO: Memory drawing.
 */
node_t *unit_test_build_with_dummy_node(void){
	node_t d;
	node_t *t = &d;
	int i;
	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_operation.print  = print_list;
	list_init.push        = push_list;

	d.link = NULL;

	for(i=1;i<6;i++){
		(*list_init.push)(&t->link,(void *)i);
		t=t->link;
	}

	(*list_operation.print)(d.link);

	return(d.link);
}


/**
 * Page 20: This time no dummy node or extra node is created.
 * Using a special reference pointer.
 * TODO: Memory drawing.
 */
node_t *unit_test_build_with_local_ref(void){
	node_t *h = NULL;
	node_t **last_ptr_ref= &h;
	int i;
	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_operation.print  = print_list;
	list_init.push        = push_list;
	list_operation.swap   = swap_2;

	for(i=1;i<6;i++){
		(*list_init.push)(last_ptr_ref,(void *)i);
		last_ptr_ref= &((*last_ptr_ref)->link);
	}

	puts("list");
	(*list_operation.print)(h);
	int n = 4;
	printf("swapping %d\n",n);
	//h = (*list_operation.swap)(h,1);
	//h = (*list_operation.swap)(h,2);
	//h = (*list_operation.swap)(h,3);
	h = (*list_operation.swap)(h,n);
	//h = (*list_operation.swap)(h,5);
	(*list_operation.print)(h);

	return h;
}


/**
 * Print a list containing addition of numbers from two lists.
 */
void unit_test_add_two_list(void){
	node_t  *h1=NULL, *h2=NULL, *h=NULL;
	node_t **r1=&h1;
	node_t **r2=&h2;

	int i;
	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_operation.print    = print_list;
	list_init.push          = push_list;
	list_operation.sum_nodes= sum_nodes;

	for(i=1;i<6;i++){
		(*list_init.push)(r1,(void *)i);
		r1= &((*r1)->link);
	}

	puts("first list");
	(*list_operation.print)(h1);

	for(i=1;i<4;i++){
		(*list_init.push)(r2,(void *)i);
		r2= &((*r2)->link);
	}
	puts("second list");
	(*list_operation.print)(h2);

	h = (*list_operation.sum_nodes)(h1,h2);
	puts("their sum");
	(*list_operation.print)(h);

	return;
}

/**
 * Prints a list containing only the odd numbers/
 */
void unit_test_odd_list(void){
	node_t *h    = NULL;
	node_t *o    = NULL;
	node_t **ref = &h;
	int i;
	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_operation.print   = print_list;
	list_init.push         = push_list;
	list_operation.odd_list= odd_list;

	for(i=1;i<6;i++){
		(*list_init.push)(ref,(void *)i);
		ref= &((*ref)->link);
	}

	puts("list");
	(*list_operation.print)(h);
	puts("list with odd numbers");
	o = (*list_operation.odd_list)(h);
	(*list_operation.print)(o);

	return;
}



/**
 * Unit test to try different techniques to do a reverse of a list
 *
 */
void unit_test_reverse(void){
	node_t *h    = NULL;
	node_t **ref = &h;
	int i;
	struct list_init_s      list_init;
	struct list_operation_s list_operation;

	list_operation.print   = print_list;
	list_init.push         = push_list;
#if(LINK_ADJUST)
	list_operation.reverse  = reverse_using_link_adjust;
#endif
	for(i=1;i<6;i++){
		(*list_init.push)(ref,(void *)i);
		ref= &((*ref)->link);
	}

	puts("list");
	(*list_operation.print)(h);
	puts("reverse of the list");
	h = (*list_operation.reverse)(h);
	(*list_operation.print)(h);

	return;
}


void unit_test_append(){
  node_t *h = NULL;
  int i;

  struct list_init_s      list_init;
  struct list_operation_s list_operation;

  list_operation.print   = print_list;
  list_init.append       = add_at_tail;
  list_init.append_after = add_after;
  list_init.push         = push_list;
  list_init.del          = del_node;
  list_init.delete_list  = del_list_2;//can be del_list_1

  for(i=1;i<6;i++){
    (*list_init.push)(&h,(void *)i);
  }

  (*list_operation.print)(h);
  h = (*list_init.append)(h,6);
  (*list_operation.print)(h);

  h = (*list_init.append_after)(h,1,7);
  h = (*list_init.append_after)(h,6,8);
  h = (*list_init.append_after)(h,5,10);
  (*list_operation.print)(h);

  /* deleting middle, first and last element */
  h = (*list_init.del)(h,6);
  h = (*list_init.del)(h,5);
  h = (*list_init.del)(h,8);
  (*list_operation.print)(h);

  (*list_init.delete_list)(h);

  return;
}

void unit_test_recursive(void){
  struct list_init_s      list_init;
  struct list_operation_s list_operation;

  list_init.build       = build_one_two_three;
  //list_operation.length = recursive_length_1;
  list_operation.length = recursive_length_2;
  list_operation.print  = print_list;
  list_init.append      = recursive_add_at_tail;

  node_t *t = NULL;

  t = (*list_init.build)();
  printf("length=%d\n",(*list_operation.length)(t));
  (*list_operation.print)(t);
  t = (*list_init.append)(t,8);
  t = (*list_init.append)(t,5);
  (*list_operation.print)(t);


  return;
}
