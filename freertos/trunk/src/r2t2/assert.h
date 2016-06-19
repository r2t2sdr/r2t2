#ifndef _ASSERT_H_
#define _ASSERT_H_

extern void vAssertCalled( const char * pcFile, unsigned long ulLine );

#define assert(x) if((x) == 0 ) vAssertCalled(__FILE__, __LINE__);


#endif
