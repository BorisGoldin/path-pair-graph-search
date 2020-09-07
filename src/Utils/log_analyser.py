import json
import argparse
import math

def analyize_averages(filename):
	analysis = {}

	with open(filename, 'r') as file:
		logs = json.load(file)

	for log in logs:
		algo = log["search_info"]["name"]
		eps = log["search_info"]["eps"][0]

		if algo not in analysis:
			analysis[algo] = {}

		if eps not in analysis[algo]:
			analysis[algo][eps] = {
				"queries_count": 0,
				"total_solutions": 0,
				"total_runtime": 0,
				"min_runtime": math.inf,
				"max_runtime": 0,
			}

		analysis[algo][eps]["queries_count"] += 1
		analysis[algo][eps]["total_solutions"] += log["finish_info"]["amount_of_solutions"]
		analysis[algo][eps]["total_runtime"] += log["total_runtime(ms)"]
		analysis[algo][eps]["min_runtime"] = min(analysis[algo][eps]["min_runtime"], log["total_runtime(ms)"])
		analysis[algo][eps]["max_runtime"] = max(analysis[algo][eps]["max_runtime"], log["total_runtime(ms)"])

	print(analysis)

	for algo in analysis:
		print(algo)
		for eps in analysis[algo]:
			analysis[algo][eps]["avg_solutions"] = \
				analysis[algo][eps]["total_solutions"]/analysis[algo][eps]["queries_count"]
			analysis[algo][eps]["avg_runtime"] = \
				analysis[algo][eps]["total_runtime"]/analysis[algo][eps]["queries_count"]

	print(json.dumps(analysis, indent=4))
	with open("analysis_"+filename, 'w') as file:
		file.write(json.dumps(analysis, indent=4))


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Analyze log files of the search algorithms")
	parser.add_argument("log_filename")

	args = parser.parse_args()
	analyize_averages(args.log_filename)
