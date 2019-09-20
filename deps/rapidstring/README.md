# rapidstring library

We have modified the [original implementation](https://raw.githubusercontent.com/boyerjohn/rapidstring/master/include/rapidstring.h)
by adding an explicit type conversion:

```
1324c1324
<       return RS_HEAP_LIKELY(s->heap.flag == RS_HEAP_FLAG);
---
>       return (unsigned char) RS_HEAP_LIKELY(s->heap.flag == RS_HEAP_FLAG);
```
