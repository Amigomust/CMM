#!/bin/bash

# 设置文件夹路径和输出文件夹路径
folder="cases"
output_folder="output"
answer_folder="answers"

# 遍历文件夹内所有以.c结尾的文件
for file in $folder/*.c; do
    # 提取文件名和扩展名
    filename=$(basename "$file")
    extension="${filename##*.}"
    filename="${filename%.*}"
    echo $file
    # 运行csubsub命令并将结果保存到输出文件
    ./Compilerlab4 "$file" > "$output_folder/$filename.out"

    # # 使用spim运行输出文件，并将结果保存到结果文件
    spim -file "$output_folder/$filename.out" | tail -n +6 > "$output_folder/$filename.res"

    # # 对比答案文件
    echo "case: $filename"
    echo "my output: "
    cat "$output_folder/$filename.res"
    echo "answer: "
    cat "$answer_folder/$filename.ans"
    
done