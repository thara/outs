require 'rake/clean'

CLEAN.include('*.o')
CLOBBER.include('outs')
CLOBBER.include('init_outs')

source_files = Rake::FileList["*.c"]
object_files = source_files.ext(".o")

task :init_outs do
  sh "gcc init_outs.c -o init_outs"
end

# task :outs => object_files do
#     sh "gcc #{object_files} -o outs"
# end

rule '.o' => '.c' do |task|
    sh "gcc -c #{task.source}"
end

task :default => :outs
