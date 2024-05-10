import subprocess
import re

# 设置参数
matrix_dimension = 1680
processor_sizes = [1, 4, 9, 16, 25, 36, 49, 64, 100, 144, 196, 225, 256]
output_file = "timing_results.txt"

# 准备输出文件
with open(output_file, "w") as f:
    f.write(f"Timing results for matrix dimension {matrix_dimension}\n")
    f.write("-------------------------------------------------\n")
    
    # 遍历每个处理器大小
    for size in processor_sizes:
        total_time = 0.0

        # 对每个处理器大小重复运行 10 次
        for _ in range(10):
            # 构建并运行命令
            command = f"srun -n {size} ./fox.exe {matrix_dimension}"
            result = subprocess.run(command, shell=True, text=True, capture_output=True)
            
            # 提取运行时间
            search = re.search(r"Total time: (\d+\.\d+) seconds", result.stdout)
            if search:
                runtime = float(search.group(1))
                total_time += runtime

        # 计算平均运行时间
        average_time = total_time / 10

        # 将结果写入文件
        f.write(f"Processor size: {size}\n")
        f.write(f"Average elapsed time: {average_time} seconds\n")
        f.write("-------------------------------------------------\n")