c = 0;
if `uname -a`.match(/ubuntu\-vm/)
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm ../so-2011/tmp/*`
  while c < 10 do
    Process.waitpid(fork { exec("true && ./tp1_vm.sh #{ARGV[0]} nocomp") })
    c += 1
  end
else 
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  while c < 10 do
    Process.waitpid(fork { exec("true && ./tp1.sh #{ARGV[0]} nocomp") })
    c += 1
  end
end