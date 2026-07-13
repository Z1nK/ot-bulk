# Module for copying TSV files to build directory
# Usage: copy_tsv_files(output_directory)

function(copy_tsv_files OUTPUT_DIR)
    file(GLOB TSV_FILES "*.tsv")
    foreach(TSV_FILE ${TSV_FILES})
        get_filename_component(FILENAME ${TSV_FILE} NAME)
        set(DEST_FILE "${OUTPUT_DIR}/${FILENAME}")
        if(NOT EXISTS ${DEST_FILE})
            configure_file(${TSV_FILE} ${DEST_FILE} COPYONLY)
        endif()
    endforeach()
endfunction()