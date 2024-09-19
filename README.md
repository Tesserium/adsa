# ADSA for DCSS 0.11+
Tesserium ADSA is a tool that aims to automatically downloads morgue files from various known sites accoding to a given username and analyzes then to give statistic data, and also some funny results.

Bloatcrawl and Bcrawl games may glitch because of various code deficiency. See [bcrawl](https://github.com/Tesserium/adsa/labels/bcrawl) & [bloatcrawl](https://github.com/Tesserium/adsa/labels/bloatcrawl) labels for more.

Usage:
`./adsa -[Oahkmp01234,] --<username>`
+ `O<filename>` => file to output results 
+ `a` => akrasiac
+ `h` => help
+ `k` => kelbi
+ `m` => morgue txts (default behavior)
+ `p` => project357
+ `0` => verbosity 0
+ `1` => verbosity 1
+ `2` => verbosity 2
+ `3` => verbosity 3
+ `4` => verbosity 4, for developer only (prints a good deal of stuff!)
+ `,` => debug mode, for developer only

Or do `./adsa -h`.

If you don't specify any username the author's (@code2828) will be used by default.

Example output (created by command `adsa -pOtestout.txt`, tabstop = 8):
```
Score		XL	Playtime		Combo			Version
43232		14	2022.12.06-01:25:32	GrFi^Gozag		30-a
35211		13	2022.12.05-08:26:57	MiBe^Trog		30-a
8272		11	2022.11.23-09:16:52	MiBe^Trog		30-a
5099		10	2022.11.23-01:38:10	AtBe^Trog		30-a
4216		10	2022.11.24-01:50:14	MiBe^Trog		30-a
3854		9	2022.12.05-07:20:06	MiBe^Trog		30-a
1818		8	2022.11.24-00:24:05	MiBe^Trog		30-a
1275		8	2022.12.06-11:52:23	MiHu^Makhleb		30-a
1257		8	2022.11.24-01:04:41	AtBe^Trog		30-a
1047		8	2022.11.24-00:46:23	MiBe^Trog		30-a
932		7	2022.11.30-00:22:57	MiBe^Trog		30-a
917		7	2022.12.06-12:07:11	MiHu			30-a
647		7	2022.12.06-03:28:36	GrEE^Sif		30-a
515		7	2022.10.20-11:01:15	DrIE^Veh		30-a
230		5	2022.11.23-00:27:01	DrIE			30-a
191		5	2022.11.29-23:59:14	MiBe^Trog		28.0 (webtiles) character file.
174		5	2022.12.06-12:30:41	OgBe^Trog		30-a
134		4	2022.11.24-01:20:41	AtBe^Trog		30-a
110		4	2022.11.24-01:11:38	AtBe^Trog		30-a
71		4	2022.11.30-00:04:21	MiBe^Trog		28.0 (webtiles) character file.
57		3	2022.11.05-13:51:09	AtHs			30-a
56		3	2022.11.05-13:29:51	AtHs			30-a
51		3	2022.11.24-00:30:20	MiBe^Trog		30-a
44		3	2022.12.06-11:31:00	DEHs			30-a
16		2	2022.12.09-00:15:26	OpHu			30-a
13		2	2022.11.23-01:00:22	AtBe^Trog		30-a
11		2	2022.11.23-00:53:43	AtBe^Trog		30-a
6		1	2022.11.04-12:27:40	DrIE			30-a
4		1	2022.11.23-00:34:10	DEFE			30-a
4		1	2022.11.24-01:05:29	AtBe^Trog		30-a
4		1	2022.12.07-00:16:08	GrFi			30-a
1		1	2022.11.23-00:31:49	DrFE			30-a
Average score: 3420.91 pts.
You have worshipped Trog for 18 times, which is the most among all gods.
```
