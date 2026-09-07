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
| <kbd>M</kbd>   | Toggle the parameter menu       |
| <kbd>j</kbd>/<kbd>k</kbd> | Select parameter (menu open) |
| <kbd>h</kbd>/<kbd>l</kbd> | Adjust by one step (menu open) |
| <kbd>H</kbd>/<kbd>L</kbd> | Adjust by ten steps (menu open) |
| <kbd>F11</kbd> | Toggle fullscreen               |
| <kbd>Esc</kbd> | Quit                            |


## Some design notes and choices

- **Stateless hashing.** `wg_hash2(seed, x, y)` gives the same answer no matter when or in what order it's called, so any tile can be regenerated in isolation. Reason is chunked or potential infinite worldgen in the future maybe.
- **Seeds chain.** Pressing `G` derives the next seed deterministically.
- **Tiles are a `uint8_t`.** So far for simplicity, this might be changed, but for these early version it was a simple solution.

## How the actual worldgen works

Every tile starts from a hash function, you give `wg_hash2(seed, x, y)` a coordinate and it gives back a number.

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

It looked like TV static at first, because it was static. 
The hash gives independence between adjacent tiles, and it's what I wanted from this hash and it guarantees it.
Terrain needs the opposite: **spatial correlation**. Every world-generation algorithm ever written is some scheme for adding correlation to independent noise. That's what the next section is.

### Adding the correlation

The hash gives me a random number per tile with no relationship to its neighbours.
Terrain howver doesn't look like that at all, so if this tile is a mountain, the one next to it is probably also a mountain. And I need this probably to be determined somehow.
**Value noise** does it at the source. Instead of hashing every tile, I hash only the corners of a coarse grid, we can say every 50th tile, and everything in between gets a blend of the nearest four corners. 
Tiles that are close together read from the same corners so they come out similar.

How you blend matters more than I expected.
A straight linear blend leaves visible creases along the grid lines, because the slope changes abruptly right at each corner and your eye picks that up immediately. 
Smootherstep (`6t⁵ − 15t⁴ + 10t³`) eases in and out at both ends so the slope matches up across corners, and the creases vanish.
**fBm** stacks it all. One grid gives you smooth blobs and nothing else.
So I add a second layer at twice the frequency and half the strength, then a third, and so on. The big layer decides where the continents go, the small ones roughen up the coastlines without moving anything important. 
`lacunarity` is how much finer each layer gets, `gain` is how much weaker.
**Banding** turns that into tiles. The result so far is a number per tile, roughly "height". Below 0.35 is deep water, below 0.45 is water, below 0.70 is grass, the rest is stone.

### Why banding one field is the whole solution

Here's the part I liked. Because everything is cut out of *one* smooth height field, tiles that shouldn't touch **can't** touch, and I don't have to write a single rule to enforce it.
Say you're walking across the map from a spot at height 0.3 to a spot at height 0.7. The field is smooth, so somewhere along the way you have to pass through 0.5.
Which means you always walk deep water → water → grass, in order. Deep water physically cannot end up next to stone, because that would mean the height jumped a whole band in one tile.

### The mode filter, and why it stopped mattering

Each cell looks at its 3×3 neighbourhood (itself included) and becomes whichever tile is most common there. Lone stray tiles get outvoted and absorbed.
One detail: the cell has to count itself. If it doesn't, and the thing you're smoothing is less than about half full, the whole structure dissolves instead of tidying up.
I planned this back when the source was white noise and there was a lot of speckle to clean. Now that it's fBm, there's almost nothing left for it to do:

| passes | landmasses | deep water | water | grass | stone |
|---|---|---|---|---|---|
| 0 | 5 | 13.9% | 22.3% | 53.8% | 10.0% |
| 1 | 4 | 13.8% | 22.3% | 53.9% | 10.0% |
| 2–5 | 4 | 13.8% | 22.3% | 53.9% | 10.0% |

The first pass cleans up a couple of stray tiles. After that nothing interesting changes. 
I kept the setting because it's one loop and it's interesting to watch, but the smooth terrain is coming from the fBm.

Worth knowing: this filter only ever looks at a tile's immediate neighbours, so it can tidy things up but it can never enforce a rule about what's allowed to touch what. That job belongs to the banding.
