all:
	@echo "READSLAM: Compilation started"
	g++ -O3 -o ./bin/preprocess ./headers/main/preprocess.cpp
	g++ -O3 -o ./bin/postprocess ./headers/main/postprocess.cpp
	g++ -O3 -o ./bin/mapper ./headers/main/map.cpp
	g++ -O3 -o ./bin/final2slam ./headers/main/final2slam.cpp
	g++ -O3 -o ./bin/sort_name ./headers/main/sort_name.cpp
	g++ -O3 -o ./bin/sort_sequence ./headers/main/sort_sequence.cpp
	g++ -O3 -o ./bin/sort_location ./headers/main/sort_location.cpp
	g++ -O3 -o ./bin/sort_clonal ./headers/main/sort_clonal.cpp
	g++ -O3 -o ./bin/collapse_clones ./headers/main/collapse_clones.cpp
	g++ -O3 -o ./bin/collapse_sorted_clones ./headers/main/collapse_sorted_clones.cpp
	g++ -O3 -o ./bin/split ./headers/main/split.cpp
	g++ -O3 -o ./bin/stack ./headers/main/stack.cpp
	g++ -O3 -o ./bin/split_stacks ./headers/main/split_stacks.cpp
	g++ -O3 -o ./bin/parse ./headers/main/parse.cpp
	g++ -O3 -o ./bin/fasta ./headers/main/fasta.cpp	
	g++ -O3 -o ./bin/test ./headers/main/test2.cpp	
	g++ -O3 -o ./bin/lookup ./headers/tools/_lookup.cpp
	g++ -O3 -o ./bin/trim ./headers/main/trim.cpp
	g++ -O3 -o ./bin/hammer ./headers/main/hammer.cpp
	g++ -O3 -o ./bin/benjamini ./headers/main/benjamini.cpp
	g++ -O3 -o ./bin/mc ./headers/main/mc.cpp
	g++ -O3 -o ./bin/methstat ./headers/main/methstat.cpp
	g++ -O3 -o ./bin/smrna ./headers/main/smrna.cpp
	@echo "READSLAM: Compilation finished"
	
old:
	#g++ -O3 -lpthread -o ./map_se ./src/mapperSE.cpp
	#g++ -O3 -lpthread -o ./bin/readslam ./headers/main/cli.cpp
	
test:
	g++ -O3 -o ./bin/benjamini ./headers/main/benjamini.cpp
	#g++ -O3 -o ./bin/bw_encode ./headers/main/bw_encode.cpp
	#g++ -O3 -o ./bin/bw_decode ./headers/main/bw_decode.cpp
	
	
todo:
	@#g++ -O3 -o ./bin/preprocess ./headers/main/preprocess.cpp
	@#g++ -O3 -o ./bin/postprocess ./headers/main/postprocess.cpp
	@#g++ -O3 -o ./bin/mapper ./headers/main/map.cpp
	@#g++ -O3 -o ./bin/sort_sequence ./headers/main/sort_sequence.cpp
	@#g++ -O3 -o ./bin/sort_location ./headers/main/sort_location.cpp
	@#g++ -O3 -o ./bin/sort_clonal ./headers/main/sort_clonal.cpp
	@#g++ -O3 -o ./bin/collapse_clones ./headers/main/collapse_clones.cpp
	@#g++ -O3 -o ./bin/collapse_sorted_clones ./headers/main/collapse_sorted_clones.cpp
	@#g++ -O3 -o ./bin/split ./headers/main/split.cpp
	@#g++ -O3 -o ./bin/stack ./headers/main/stack.cpp
	@#g++ -O3 -o ./bin/split_stacks ./headers/main/split_stacks.cpp
	@#g++ -O3 -o ./bin/fasta ./headers/main/fasta.cpp	
	@#g++ -O3 -o ./bin/test ./headers/main/test2.cpp	
	@#g++ -O3 -o ./bin/lookup ./headers/tools/_lookup.cpp
	@#g++ -O3 -o ./bin/trim ./headers/main/trim.cpp
	@#g++ -O3 -o ./bin/hammer ./headers/main/hammer.cpp
	@#g++ -O3 -o ./bin/hammer ./headers/main/benjamini.cpp
	@#g++ -O3 -o ./bin/mc ./headers/main/mc.cpp
	@#g++ -O3 -o ./bin/methstat ./headers/main/methstat.cpp
	@#g++ -O3 -o ./bin/smrna ./headers/main/smrna.cpp
	
done:
	g++ -O3 -o ./bin/parse ./headers/main/parse.cpp
	