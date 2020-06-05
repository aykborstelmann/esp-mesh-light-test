# ESP-Mesh Test

## What happens
The master switches its light and sends a broadcast message to all node, to follow him.

## Reason
I just wanted to test the mesh communication for ESPs. 
I found the library [painlessMesh](https://platformio.org/lib/show/1269/painlessMesh) and used it for this project.

## Remember
Synchronisation of time is a actual feature of the painlessMesh-library. 
So my way of synchronizing switching of lights is not an real best practice for this library.
