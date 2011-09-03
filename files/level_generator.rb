# Made by Cristian Pereyra - WTFPL
# /* This program is free software. It comes without any warranty, to
#  * the extent permitted by applicable law. You can redistribute it
#  * and/or modify it under the terms of the Do What The Fuck You Want
#  * To Public License, Version 2, as published by Sam Hocevar. See
#  * http://sam.zoy.org/wtfpl/COPYING for more details. */

class Array
  def randomize!
    self.sort! { rand(100) - 50 }
  end
end


  puts "Mode of use: ruby level_generator.rb NUMBER_OF_CITIES NUMBER_OF_MEDICINES NUMBER_OF_AIRLINES MAX_MED_AMOUNT ARC_PROBABILITY MAX_DISTANCE PLANES FOLDER_NAME"
  puts "Builds a folder with all the files to pass to the TP"

  
  # This medicines and cities are just taken from google, feel free to change em.
  meds = ["Adiro100", "Nolotil", "Efferalgan", "Gelocatil", "Augmentine", "VoltarenEmulgel", "Lexatn", "Paracetamol", "Orfidal", "Dianben", "Neobrufen", "Trankimazn", "Ventoln", "Almax", "Flumil", "Sintrom", "Termalgin", "Viscofresh", "Enantyum", "Cardyl"] 
  cities = ["Abidjan","Ahmedabad","Alexandria","Ankara","Baghdad","Bangalore","Bangkok","Beijing","Berlin","Bogota","BuenosAires","Busan","Cairo","CapeTown","Chennai","Chongqing","Delhi","Dhaka","Dongguan","Durban","Guangzhou","Hanoi","HoChiMinhCity","HongKong","Hyderabad","Istanbul","Jaipur","Jakarta","Jeddah","Johannesburg","Kanpur","Karachi","Kinshasa","Kolkata","Lagos","Lahore","Lima","London","LosAngeles","Madrid","MexicoCity","Moscow","Mumbai","Nairobi","NewYorkCity","Pune","Pyongyang","RioDeJaneiro","Riyadh","SaintPetersburg","Santiago","Seoul","Shanghai","Shenyang","Shenzhen","Singapore","Surat","SaoPaulo","Tehran","Tianjin","Tokyo","Wuhan","Yangon","Yokohama"]

if ARGV.count >= 7  
  n_cities =      ARGV[0].to_i
  n_med =         ARGV[1].to_i
  n_air =         ARGV[2].to_i
  max_med =       ARGV[3].to_i
  arc_prob =      ARGV[4].to_f
  max_distance =  ARGV[5].to_i
  n_planes =    ARGV[6].to_i
  folder =        ARGV[7]
else
   n_cities =      50
   n_med =         15
   n_air =         10
   max_med =       1000
   arc_prob =      1
   max_distance =  30
   n_planes =    10
   folder =        "default"
end
  
  relations = {}
  
  selected_meds = []
  selected_cities = []
  
  meds.randomize!
  cities.randomize!
  
  n_cities.times do
    break if cities.size == 0  
    selected_cities.push cities.pop
  end
  n_med.times do
    break if cities.size == 0
    selected_meds.push meds.pop
  end
  


  Dir.mkdir folder unless test ?d, folder
  path = "#{folder}"
  map_name = "#{path}/mapa.txt"
  map_f = File.open(map_name, 'w')
  
  map_f.puts n_cities

  selected_cities.each do |city|
    map_f.puts
    map_f.puts city
    c = rand(n_med - 1) + 1
    meds = selected_meds.clone.randomize!
    c.times do 
      map_f.puts "#{meds.pop} #{rand(max_med - 10) / 10 * 10 + 10}"
    end
  end
  
  first = true
  selected_cities.each do |from|
    map_f.puts
    selected_cities.each do |to|
      if (rand() < arc_prob or first)  and from != to
        if relations[from] == nil
          relations[from] = []
        end
        unless relations[from].include? to or (not relations[to].nil? and relations[to].include? from)

          relations[from].push to
        
          map_f.puts "#{from} #{to} #{(rand(max_distance - 10) + 10) / 5 * 5}"
        end
      end
    end
    first = false
  end
  map_f.close

  airline_files = []
  
  n_air.times do |i|
    air_name = "#{path}/aerolinea#{i}.txt"
    air_f = File.open(air_name, 'w')
    

    cities = selected_cities.clone.randomize!

    air_f.puts n_planes

    n_planes.times do
      meds = selected_meds.clone.randomize!
      air_f.puts
      air_f.puts cities.pop
      n_planes.times do
        air_f.puts "#{meds.pop} #{rand(max_med - 10) / 10 * 10 + 10}"
      end
    end
    air_f.close

    airline_files.push air_name
  end
  
  str = "./tp1 --method sockets --level files/#{map_name} --airline "
  airline_files.each do |air|
    str << " files/#{air}"
  end
    
  `echo #{str} | pbcopy`
  