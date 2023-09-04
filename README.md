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
Score		Turn	Score per turn	XL	Playtime		Combo			Version
43232		16592	2.60559		14	2022.12.06-01:25:32	GrFi^Gozag		30-a
35211		13401	2.62749		13	2022.12.05-08:26:57	MiBe^Trog		30-a
8272		7887	1.04881		11	2022.11.23-09:16:52	MiBe^Trog		30-a
5099		8106	0.62904		10	2022.11.23-01:38:10	AtBe^Trog		30-a
4216		6778	0.62201		10	2022.11.24-01:50:14	MiBe^Trog		30-a
3854		7650	0.50379		9	2022.12.05-07:20:06	MiBe^Trog		30-a
1818		4587	0.39634		8	2022.11.24-00:24:05	MiBe^Trog		30-a
1275		3518	0.36242		8	2022.12.06-11:52:23	MiHu^Makhleb		30-a
1257		3244	0.38748		8	2022.11.24-01:04:41	AtBe^Trog		30-a
1047		4219	0.24816		8	2022.11.24-00:46:23	MiBe^Trog		30-a
932		4866	0.19153		7	2022.11.30-00:22:57	MiBe^Trog		30-a
917		4312	0.21266		7	2022.12.06-12:07:11	MiHu			30-a
647		3436	0.1883		7	2022.12.06-03:28:36	GrEE^Sif		30-a
515		2604	0.19777		7	2022.10.20-11:01:15	DrIE^Veh		30-a
230		3305	0.06959		5	2022.11.23-00:27:01	DrIE			30-a
191		1690	0.11302		5	2022.11.29-23:59:14	MiBe^Trog		28.0 (webtiles) character file.
174		2585	0.06731		5	2022.12.06-12:30:41	OgBe^Trog		30-a
134		2340	0.05726		4	2022.11.24-01:20:41	AtBe^Trog		30-a
110		1937	0.05679		4	2022.11.24-01:11:38	AtBe^Trog		30-a
71		2789	0.02546		4	2022.11.30-00:04:21	MiBe^Trog		28.0 (webtiles) character file.
57		1090	0.05229		3	2022.11.05-13:51:09	AtHs			30-a
56		802	0.06983		3	2022.11.05-13:29:51	AtHs			30-a
51		1671	0.03052		3	2022.11.24-00:30:20	MiBe^Trog		30-a
44		1153	0.03816		3	2022.12.06-11:31:00	DEHs			30-a
16		791	0.02023		2	2022.12.09-00:15:26	OpHu			30-a
13		757	0.01717		2	2022.11.23-01:00:22	AtBe^Trog		30-a
11		159	0.06918		2	2022.11.23-00:53:43	AtBe^Trog		30-a
6		385	0.01558		1	2022.11.04-12:27:40	DrIE			30-a
4		27	0.14815		1	2022.11.23-00:34:10	DEFE			30-a
4		58	0.06897		1	2022.11.24-01:05:29	AtBe^Trog		30-a
4		176	0.02273		1	2022.12.07-00:16:08	GrFi			30-a
1		96	0.01042		1	2022.11.23-00:31:49	DrFE			30-a
Average score: 3420.91 pts.
You have worshipped Trog for 18 times, which is the most among all gods.
```

## Contribution
The best way to contribute to this project is to join Tesserium! E-mail me <tmp_0x02@null.net> or any owner of the team for more details.

