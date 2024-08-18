/*
 * main.c
 *
 *  Created on: Feb 3, 2011
 *      Author: ajay
 */
#include <stdio.h>

//Instances of this struct will be my objects
struct foo {
    //Properties
    int bar;

    //Methods
    void (* print)(struct foo self);
    void (* printSum)(struct foo self, int delta);
};

//Here is the actual implementation of the methods
static void printFoo(struct foo self) {
    printf("This is bar: %d\n", self.bar);
}

static void printSumFoo(struct foo self, int delta) {
    printf("This is bar plus delta: %d\n", self.bar + delta);
}

//This is a sort of constructor
struct foo Foo(int bar) {
    struct foo foo = {
        .bar = bar,
        .print = &printFoo,
        .printSum = &printSumFoo
    };
    return foo;
}

//Finally, this is how one calls the methods
void
main(void) {
    struct foo foo = Foo(14);
    foo.print(foo); // This is bar: 14
    foo.printSum(foo, 2); // This is bar plus delta: 16
}

