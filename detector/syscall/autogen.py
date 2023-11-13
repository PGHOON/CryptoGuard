def save_formatted_code_with_trace_syscall(input_file_path, output_file_path):
    with open(input_file_path, 'r') as input_file, open(output_file_path, 'w') as output_file:
        for line in input_file:
            parts = line.strip().split()
            if not parts:
                continue

            syscall = parts[0]
            tp_name = 'tp_' + syscall.split('_')[-1]

            formatted_string = f"""SEC("tp/syscalls/{syscall}")
TRACE_SYSCALL({syscall}, "{tp_name}")\n"""
            output_file.write(formatted_string)

input_file_path = 'enter_text.txt'
output_file_path = 'SEC_exit_Autogen.txt'
save_formatted_code_with_trace_syscall(input_file_path, output_file_path)
