# Toolchain script to perform data analysis 
# depends on extenal tool (here called "log_parser.exe")
# to convert from raw log file to properly formatted average.exe input

# Safety checks
if ( $args.Length -lt 2 ) {
  Write-Host "Usage:    $($MyInvocation.MyCommand.Name) [device_type_char] path/to/log_file"
  Exit 7
}

# Preparing standard name and variables
$log_file_name     = $args[1]
$device_switch     = "-$($args[0])"
$log_file_basename = $log_file_name.Substring(0,$log_file_name.length-4)
$ave_file_name     = $log_file_basename + "_inertial.txt"
$plot_script_name  = $log_file_basename + "_inertial_ave.plt"

# Calling raw data parser and clean up
$log_parser_call = "log_parser.exe $device_switch $log_file_name"
Write-Host $log_parser_call
Invoke-Expression $log_parser_call
Remove-Item -Force *_acc.txt 
Remove-Item -Force *_error.txt 
Remove-Item -Force *_err.txt 
Remove-Item -Force *_gnss.* 
Remove-Item -Force *_gyr.txt

# Calling average
$average_call = "average.exe -period 1 $ave_file_name"
Write-Host $average_call
Invoke-Expression $average_call

# Building the image
$gnuplot_call = "gnuplot.exe $plot_script_name"
Write-Host $gnuplot_call
Invoke-Expression $gnuplot_call
