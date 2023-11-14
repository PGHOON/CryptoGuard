import pandas as pd
import sys

def main():
    syscall_csv = 'syscall.csv'
    target_csv = 'target.csv'

    headers_df = pd.read_csv(syscall_csv)
    system_call_headers = headers_df.columns.tolist()[1:]


    csv_df = pd.read_csv(target_csv)


    system_call_counts = {header: csv_df['SYSTEM_CALL'].tolist().count(header) for header in system_call_headers}
    anomality_value = int(sys.argv[1]) if len(sys.argv) > 1 else 0
    counts_df = pd.DataFrame([system_call_counts], columns=system_call_headers)
    counts_df.insert(0, 'anomality', anomality_value)

    with open(syscall_csv, 'a') as f:
        counts_df.to_csv(f, header=False, index=False)

if __name__ == "__main__":
    main()