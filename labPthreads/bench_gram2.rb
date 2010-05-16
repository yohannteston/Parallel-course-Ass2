#!/usr/local/bin/ruby

# Benchmark for gram2
# For each size and each number of thread launch 20 times
# the program and keep the least time.
[10,100,1000,10000,100000].each do |size|
	[1,4,8,12,16,24,32,48,64].each do |n|
		min_time = 10000000000 ;

		20.times {
			out = `gram2 #{size} #{n}` 
			out_split = out.split " " ;
			if(out_split[1].to_f < min_time)
				min_time = out_split[1].to_f;
			end
		}

		puts "#{size} #{n}  #{min_time}" ;
	end
end
