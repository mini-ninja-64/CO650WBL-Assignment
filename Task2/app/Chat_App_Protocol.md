For the chat app, I designed a simple packet format that could be used to 
exchange messages between client and server, it has been documented in this file

The base packet is versioned to make future revisions easier.

## Chat App Packet v1 Design:
| Section | Field                     | Type     | Size    |
|---------|---------------------------|----------|---------|
| Packet  | Packet Version            | uint8_t  | 1 byte  |
|         |                           |          |         | 
| Header  | Sender ID                 | uint16_t | 2 bytes |
|         |                           |          |         |
| Body    | Body Type                 | uint8_t  | 1 byte  |
|         | Body Data Length in bytes | uint32_t | 4 bytes |
|         | Body Data                 | unknown  | N bytes |

Body Types:
- `0x01` - ASCII Message NULL Terminator is not required & Body Data is a stream of ASCII characters
- `0x02` - Chat close request

