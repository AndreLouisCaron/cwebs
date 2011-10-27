#ifndef _owire_h__
#define _owire_h__

/*!
 * @file owire.h
 * @brief Web Socket wire protocol (out-bound) for C.
 * @see http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-17
 *
 * @author Andre Caron (andre.louis.caron@usherbrooke.ca)
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64(*ws_owire_state)
    (struct ws_owire*,const uint8*,uint64);

enum ws_owire_error
{
    ws_owire_eok,
    ws_owire_enotready,
};

  /*!
   * @brief Streaming parser for Web Socket wire protocol (out-bound).
   */
struct ws_owire
{
    // event callbacks.
    void(*accept_content)(struct ws_owire*,const void*,uint64);
    void(*random_mask)(struct ws_owire*,uint8 mask[4]);

    // for use by callbacks.
    void * baton;

    // read-only fields.
    enum ws_owire_error error;

    // public fields.
    uint64 auto_fragment;
    int auto_mask;

    // internal state.
    uint8 data[14];
    uint64 used;
    uint64 pass;
    ws_owire_state state;
};

void ws_owire_init ( struct ws_owire * stream );

void ws_owire_new_message ( struct ws_owire * stream );
void ws_owire_end_message ( struct ws_owire * stream );

void ws_owire_new_frame ( struct ws_owire * stream, uint64 size );
void ws_owire_end_frame ( struct ws_owire * stream );

void ws_owire_last ( struct ws_owire * stream );
void ws_owire_eval ( struct ws_owire * stream, uint8 eval );
void ws_owire_code ( struct ws_owire * stream, uint8 code );
void ws_owire_mask ( struct ws_owire * stream );
uint64 ws_owire_feed
    ( struct ws_owire * stream, const void * data, uint64 size );

  // Follow with calls to _feed(), _end_frame(), etc.
void ws_owire_ping ( struct ws_owire * stream );
void ws_owire_pong ( struct ws_owire * stream );
void ws_owire_text ( struct ws_owire * stream );
void ws_owire_data ( struct ws_owire * stream );
void ws_owire_kill ( struct ws_owire * stream );

  // emit full frames in a single bundle.
void ws_owire_put_text
    ( struct ws_owire * stream, const void * data, uint64 size );
void ws_owire_put_data
    ( struct ws_owire * stream, const void * data, uint64 size );
void ws_owire_put_ping
    ( struct ws_owire * stream, const void * data, uint64 size );
void ws_owire_put_pong
    ( struct ws_owire * stream, const void * data, uint64 size );
void ws_owire_put_kill
    ( struct ws_owire * stream, const void * data, uint64 size );

#ifdef __cplusplus
}
#endif

#endif /* _owire_h__ */
