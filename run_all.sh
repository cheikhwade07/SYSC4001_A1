
# ===========================================================
# SYSC4001A - Run all interrupt simulations automatically
# ===========================================================

# Ensure output folder exists
mkdir -p output_files

# Loop through each trace file
for trace in input_files/trace_*.txt; do
    base=$(basename "$trace" .txt)

    # Loop through context_time and isr_time combinations
    for context in 10 20 30; do
        for isr in 40 100 200; do
            outfile="output_files/${base}_ctx${context}_isr${isr}.txt"
            echo " Running $base with context=$context ms, ISR=$isr ms..."

            # Make sure the output file exists before running
            touch "$outfile"

            # Run the simulator
            ./bin/interrupts.exe "$trace" vector_table.txt device_table.txt \
                                 "$outfile" "$context" "$isr"

            # Check if execution succeeded
            if [ $? -ne 0 ]; then
                echo " Simulation failed for $outfile"
            fi
        done
    done
done

echo " All simulations completed. Check the output_files/ folder!"
