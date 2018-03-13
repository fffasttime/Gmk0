## Training history

### First test

Using python+tensorflow generate selfplay game.
##### default settings
playout|mse_factor|dir_noise|use_rotate|block|fliters|step|lr
-------|----------|---------|----------|-----|-------|----|--
120|1.0|0.03|False|5|48|16k|0.05

|Date|Black|White|Opp Black|Opp White|Info|Note|
|----|------|------|---------|-----|---------|----------|
|12-30||||||change conv to resdential, add bn, add dual|
|1-22|||||training SL for test|speed of mcts too slow|
|1-23|50|45|0|5|SL vs random|SL test OK|
|||||||completed NN estimate, start generate data|
|||||||optimized mcts selectiom, 2-3x speed up|
|1-24||||||selfplay data OK, but a little problem|
||||||start Iteration 0|add dir noise|
|1-25||||||restart w0, because value output reversed|
|1-26|||||w0 generated|test shows decay is important(=0.85)|
||44|41|9|6|w0 vs random|85%|
|1-27||||||optimized tree search, 3x speed up|
|1-28|32|29|21|18|w1 vs w0|61%|
|1-29|32|33|17|18|w2 vs w1|65%|
| |29|30|20|21|w3 vs w2|59%|
| |43|31|19|7|w4 vs w3|74%|
|1-30|30|29|21|20|w5 vs w4|59%|
| |32|36|14|18|w6 vs w5|68%|
|1-31|35|49|1|15|w7 vs w6|84%|
| |26|47|3|24|w8 vs w7|73%|
|1-32|26|36|14|24|w9 vs w8|62%|
| |15|45|5|35|w10 vs w9|60%|
|2-2|24|43|7|26|w11 vs w10|67%|
| |||||find huge bug in training|w1-11 will strongly effected|
| |||||fixed bug|this bug led white have high win rate|
| |41|43|7|9|w12 vs w11|84%|
| |14|14|36|36|w12 vs SL|28%, conformance to expectation|
|2-3|48|41|9|2|w13 vs w12|89%|
| |44|35|15|6|w14 vs w13|79%|
|2-4|41|35|15|9|w15 vs w14|76%|
| |45|36|14|5|w16 vs w15|81%|
| |27|23|27|23|w16 vs SL|50%, I think RL process has self overfitting|
|2-5|44|35|15|6|w17 vs w16|79%|
| |26|21|29|24|w18 vs w17|47%|
|2-6|37|24|16|13|w19 vs w18|61%|
| |36|22|28|14|w20 vs w19|58%|
| |33|31|19|17|w20 vs SL|64%, overfitting|
|2-7||||||finished cpp search|
|2-8||||||add cpp cmd lines|
|2-9||||||add gmk protrol support|
|2-10||||||stopping first test, now use cpp version to generate data|

### Formal test
Using cpp version to generate selfplay data
##### default settings
playout|mse_factor|dir_noise|use_rotate|block|fliters|step|lr
-------|----------|---------|----------|-----|-------|----|--
400|1.0|0.2|True|5->6|48->64->96|16k->4k|0.05->0.002

|Date|Black|White|Opp Black|Opp White|Info|Note|
|----|------|------|---------|-----|---------|----------|
2-11|43|34|16|7|I0 vs random|77%|
2-12||||||find bug, restart|
||42|34|16|8|I0 vs random|94%|
||48|46|4|2|I1 vs I0|94%|
|2-13|50|47|3|0|I2 vs I1|97%|
||49|40|10|1|I3 vs I2|89%|
|Valentin's Day|47|21|19|3|I4 vs I3|68%|
||46|37|13|4|I5 vs I4|83%|
|2-15|38|27|23|12|I6 vs I5|65%|
||36|21|29|14|I7 vs I6|57%|
||39|30|20|11|I8 vs I7|69%|
|Spring Festival|33|34|16|17|I9 vs I8|67%|
||42|29.5|20.5|8|I10 vs I9|71.5%|
|2-17|31|31.5|19|18.5|I11 vs I10|62.5%|
|2-18|35|36|14|15|I12 vs I11|71%|
||37|28|22|13|I13 vs I12|65%|
|2-19|35|26|-|-|I14 vs I13|61%|
||41|29|-|-|I15 vs I14|70%|
|2-20|34|31|-|-|I16 vs I15|65%|
|2-21|39|23|-|-|I17 vs I16|62%|
||39|24|-|-|I18 vs I17|63%|
|2-22|31|22|-|-|I19 vs I18|53%|
||41|26.5|-|-|I20 vs I19|67.5%, changed to 6b64f|
|2-23|38|31.5|-|-|I21 vs I20|69.5%|
||36|25|-|-|I22 vs I21|61%|
|2-24|31|26|-|-|I23 vs I22|61%|
||41|26|-|-|I24 vs I23|71%|
|2-25|45|16|-|-|I25 vs I24|61%|
|2-26|50|11.5|-|-|I26 vs I25|61.5%|
||41|19|-|-|I27 vs I26|60%|
|2-27|40|18|-|-|I28 vs I27|58%|
|2-28|40|26|-|-|I29t1 vs I28|66%, lr set to 0.005|
||41|29|-|-|I30 vs I29t1|70%, lr set to 0.002|
|3-1|41|28|-|-|I31 vs I30|69%|
||35|23|-|-|I32 vs I31|58%|
|3-2|38|17|-|-|I33 vs I32|55%|
|3-3|38|20.5|-|-|I34s vs I33|58.5%|
||41|14|-|-|I35 vs I34|55%|
|3-6|||||I36, I37|~50%, underfitting or overfitting?|
|3-8|38|19|-|-|I38s vs I37t|57%, changed to 6b96|
||36|17|-|-|I39s vs I37t|53%|
|3-9|38|16|-|-|I40s vs I39s|54%, add mc win|
|3-10|39|20|-|-|I41s vs I40s|59%|
||37|16|-|-|I42s vs I41s|53%|
|3-11|61|-|39|-|I42-4000po vs AIWine-10s|61% win normal AlphaBeta AI|
||3|-|27|-|I42-4000po vs Yixin16_64-10s|10% loss strong tradinational computer|
|3-12|||||model grow up slower, stopped|


### conclusion
- Training shows AlphaZero method is a good way to learn a good gmk AI.
- After all the model didn't add any prior knowlodge, or optimize the MCTS search for gomoku.
- If Gmk0 is combined with some tradinational method, like vct/vcf calcluation, I think it will be much stronger.Playing gomoku rely on overall view and part calculation. Neural network is good at overall view, but too slow to do simple calculation.
- And another problem, it's not easy to make gmk0 adapt with different gomoku rules. So we'd better to choose a specific rule before start training.
