c = 0;
  while c < 10 do
    Process.waitpid(fork { exec("true && ./tp1.sh #{ARGV[0]} nocomp") })
    c += 1
  end