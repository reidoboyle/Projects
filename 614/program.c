#include <stdio.h>
#include <stdlib.h>
// struct s is a doubly linked list containing an integer value and a pointer to 
// the previous list item and the next list item
struct s {
	int a;
	struct s *b, *c;
};
// function f recursively searches the linked list for the value val
// if found it returns the list item otherwise it returns the closest item in the list
struct s * f(struct s * s1, int val)
{
    if (s1->a!= val)
    {
        if (s1->a < val)
        {
            return f(s1->c,val);
        }
        else if(s1->a > val)
        {
            return f(s1->b,val);
        }
    }

    return s1;
}
// testing function f
int main(int argc,char *argv[])
{
    struct s *s1 = (struct s *) malloc(sizeof(struct s));
    struct s *orig = s1;
    s1->a = 0;
    s1->b = NULL;
    for(int i=1;i<10;i++)
    {
        struct s *s2 = (struct s *) malloc(sizeof(struct s));
        s1->c = s2;
        s2->a = i;
        s2->b = s1;
        s1 = s2;
    }

    struct s *ans = f(orig,9);
    printf("%d",ans->a);
}