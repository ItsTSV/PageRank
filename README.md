# 3. PageRank Algorithm

Z Pythonu jsem bohužel musel přejít na C++, jelikož jen samotné načítání trvalo extrémně dlouho.

V C++ jsem použil OpenMP; je tedy nutno kompilovat s flagem `-fopenmp`. Paralelní načítání a běh
algoritmu trvalo 15 sekund. V případě běhu bez paralelizace trvalo vykonání programu podstatně déle -- 70 sekund.

Výsledky byly v obou případech stejné:

```
PageRank best values:
Node 272919 -> 0.0092751
Node 438238 -> 0.00734008
Node 210376 -> 0.0050028
Node 210305 -> 0.0049205
Node 601656 -> 0.00368971
```