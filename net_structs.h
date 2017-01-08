#ifndef NET_STRUCTS_H
#define NET_STRUCTS_H

struct neuronet {
    int nl, *nn, *nw, total_nn, total_nw;
    double *w;
    double *g;
};

#endif // NET_STRUCTS_H

