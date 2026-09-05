# WORLDGEN

A sandbox program for my own world generation from scratch in C.
Not sure what I'll do with it later, so far, this is just me playing around and experimenting.

## deadbeef seed

<img width="819" height="831" alt="deadbeefseed" src="https://github.com/user-attachments/assets/80a377bd-07c2-4f35-99d0-601fa510af56" />


## Trying it out

```bash
sudo pacman -S sdl3 # for Arch
make
make run # random seed
make run ARGS=0xDEADBEEF # reproduce a specific seed for the world
make debug # ASan/UBSan build
```

> **_NOTE:_** The seed is printed in stdout and window title.

## Keys

| Key   | Action                          |
|-------|---------------------------------|
| <kbd>G</kbd>   | Generate a new world (next seed)|
| <kbd>F11</kbd> | Toggle fullscreen               |
| <kbd>Esc</kbd> | Quit                            |


## Some design notes and choices

- **Stateless hashing.** `wg_hash2(seed, x, y)` gives the same answer no matter when or in what order it's called, so any tile can be regenerated in isolation. Reason is chunked or potential infinite worldgen in the future maybe.
- **Seeds chain.** Pressing `G` derives the next seed deterministically.
- **Tiles are a `uint8_t`.** So far for simplicity, this might be changed, but for these early version it was a simple solution.

## How the actual worldgen works

Right now every tile is decided independently by a hash function, you give `wg_hash2(seed, x, y)` a coordinate and it gives back a number.

It is a stateless hash rather than a stateful PRNG. A normal random number generator is a state machine: to get the 5000th value you have to grind through the previous 4999. 
And I don't want that, so therefore this solution is: any tile can be computed on its own, in any order, on any thread, without touching the others.
Formally that's the difference between a pseudorandom generator and a pseudorandom function. Practically it means chunked or infinite worlds cost nothing extra later, and it's impossible to write an ordering bug. Which I bet I would do at some point.

The mixing function itself is `lowbias32`, found by Chris Wellons' [hash prospector](https://github.com/skeeto/hash-prospector), which is a tool that generated billions of candidate hash functions and scored them on how well they scramble. 
I didn't invent the constants and wouldn't have found them by hand, they came out of a search. Public domain, but credit where credit is due.
Two properties make it work, and it took me a while to see they were separate things:

- **It's a bijection.** Every operation in it is reversible, xorshift is invertible, and multiplying by an odd constant is invertible mod 2^32 because odd numbers are units in that ring. 
So the whole thing is a permutation of all 2^32 integers without collisions and no value favoured over another.

- **It avalanches.** Flip one bit of the input and every output bit flips with probability ~1/2. This is the Strict Avalanche Criterion.

Bijection alone is not enough, which surprised me. 
The identity function is a perfect bijection and generates a completely empty world. Multiply-by-an-odd-constant is a perfect bijection, hits 49.99% stone, and draws a neat checkerboard.

It currently looks like TV static because it currently is static, this will change obviously. 
The hash gives independence between adjacent tiles, and it's what I wanted from this hash and it guarantees it.
Terrain needs the opposite: **spatial correlation**. Every world-generation algorithm ever written is some scheme for adding correlation to independent noise. That's the next thing to build.
