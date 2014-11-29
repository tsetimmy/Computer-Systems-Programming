make clean && make && ./randtrack 1 50 > rt1.out && sort -n rt1.out > rt1.outs && ./randtrack_global_lock 2 50 > rt2.out && sort -n rt2.out > rt2.outs
diff rt1.outs rt2.outs
rm rt1.out rt2.out rt1.outs rt2.outs

make clean && make && ./randtrack 1 50 > rt1.out && sort -n rt1.out > rt1.outs && ./randtrack_tm 2 50 > rt2.out && sort -n rt2.out > rt2.outs
diff rt1.outs rt2.outs
rm rt1.out rt2.out rt1.outs rt2.outs

make clean && make && ./randtrack 1 50 > rt1.out && sort -n rt1.out > rt1.outs && ./randtrack_list_lock 2 50 > rt2.out && sort -n rt2.out > rt2.outs
diff rt1.outs rt2.outs
rm rt1.out rt2.out rt1.outs rt2.outs

make clean && make && ./randtrack 1 50 > rt1.out && sort -n rt1.out > rt1.outs && ./randtrack_element_lock 2 50 > rt2.out && sort -n rt2.out > rt2.outs
diff rt1.outs rt2.outs
rm rt1.out rt2.out rt1.outs rt2.outs

make clean && make && ./randtrack 1 50 > rt1.out && sort -n rt1.out > rt1.outs && ./randtrack_reduction 2 50 > rt2.out && sort -n rt2.out > rt2.outs
diff rt1.outs rt2.outs
rm rt1.out rt2.out rt1.outs rt2.outs

