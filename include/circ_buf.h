// Circular buffer of DOUBLE values (which of course can be ints as well) for the purpose of computing a running mean. 
// Note this does NOT include the functionality of "get next element", and overflow just overwrites the oldest element.

typedef struct _circ_buf {
    int len, in, out, n;
    double *buf, sum;
} CIRC_BUF;

CIRC_BUF *CircBufAlloc(int len); // len = physical size of the buffer
void CircBufReset(CIRC_BUF *c);
double CircBufAdd(CIRC_BUF *c, double val); // returns the mean
double CircBufMean(CIRC_BUF *c);
