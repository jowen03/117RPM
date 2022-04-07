#!/bin/env ruby
#
#          print signatures of all the functions named in supplied IDL file
#

require 'json'

IDL_TO_JSON_EXECUTABLE = './idl_to_json'

#
#     Make sure invoked properly
#

abort "Usage: #{$PROGRAM_NAME} <idlfilename>" if ARGV.length != 1


#
#     Make sure file exists and is readable
#
filename = ARGV[0]
abort "#{$PROGRAM_NAME}: no file named #{filename}" if not File.file? filename
abort "#{$PROGRAM_NAME}: #{filename} not readable" if not File.readable? filename

#
#     Parse declarations into Ruby hash
#
if !File.executable?(IDL_TO_JSON_EXECUTABLE)
  abort "#{IDL_TO_JSON_EXECUTABLE} does not exist or is not executable..."
end
json_string =`idl_to_json #{filename}`
abort "#{$PROGRAM_NAME}: Failed to parse IDL file #{filename}" if $? != 0
decls =  JSON.parse(json_string)

#
#     Print the function signatures
#
decls["functions"].each do |name, sig|

  # Ruby Array of all args (each is a hash with keys "name" and "type")
  args = sig["arguments"]

  # Make a string of form:  "type1 arg1, type2 arg2" for use in function sig
  argstring = args.map{|a| "#{a["type"]} #{a["name"]}"}.join(', ')

  # print the function signature
  puts "#{sig["return_type"]} #{name}(#{argstring})"

end
