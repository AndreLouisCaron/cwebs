#ifndef _iwire_h__
#define _iwire_h__

/*!
 * @file iwire.h
 * @brief Web Socket wire protocol (in-bound) for C.
 * @see http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-17
 *
 * @author Andre Caron (andre.louis.caron@usherbrooke.ca)
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ws_iwire;

typedef uint64(*ws_iwire_state)
    (struct ws_iwire*,const uint8*,uint64);

  /*!
   * @brief Streaming parser for Web Socket wire protocol (in-bound).
   */
struct ws_iwire
{
    // event callbacks.
    void(*new_message)(struct ws_iwire*);
    void(*end_message)(struct ws_iwire*);
    void(*new_fragment)(struct ws_iwire*,uint64);
    void(*end_fragment)(struct ws_iwire*);
    void(*accept_content)(struct ws_iwire*,const void*,uint64);

    // for use by callbacks.
    void * baton;

    // read-only fields.
    int code;
    int eval;
    int last;
    int usem;
    int good;

    // internal state.
    ws_iwire_state state;
    uint8 data[8];
    uint64 size;
    uint8 mask[4];
    uint64 used;
    uint64 pass;
};

void ws_iwire_init ( struct ws_iwire * stream );

uint64 ws_iwire_feed
    ( struct ws_iwire * stream, const void * data, uint64 size );

int ws_iwire_ping ( const struct ws_iwire * stream );
int ws_iwire_pong ( const struct ws_iwire * stream );

int ws_iwire_text ( const struct ws_iwire * stream );
int ws_iwire_data ( const struct ws_iwire * stream );

int ws_iwire_dead ( const struct ws_iwire * stream );

#ifdef __cplusplus
}
#endif

#endif /* _iwire_h__ */

