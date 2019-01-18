import os.path
import sys
import os


def count_trigrams(filepath):
    try:
        with open(filepath, "rb") as file:
            content = file.read()
    except Exception as e:
        return -1, 0
    ans = 0
    trigrams = set()
    for i in range(len(content) - 2):
        trigram = (content[i], content[i + 1], content[i + 2])
        if trigram not in trigrams:
            ans += 1
            trigrams.add(trigram)
    return ans, len(content)


if __name__ == "__main__":
    count_list = []
    a = []

    for dirpath, dirnames, filenames in os.walk(os.getcwd()):
        for file in filenames:
            fullpath = os.path.join(dirpath, file)
            cnt, size = count_trigrams(fullpath)
            if cnt == -1 or size == 0:
                continue
            count_list.append(cnt)
            a.append(cnt / size)
            print(fullpath, cnt, size, cnt / size)

    count_list.sort()
    a.sort()
    print("Average:", sum(count_list) // len(count_list))
    print("Median:", count_list[len(count_list) // 2])
    print(a)
