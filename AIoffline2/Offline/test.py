# import random
# import urllib.request
#
# def download_web_image(url):
#     name= random.randrange(1,1000)
#     full_name = str(name)+".jpg"
#     urllib.request.urlretrieve(url, full_name)
#
# url = "https://i.ytimg.com/vi/EfYoA36N9nM/hqdefault.jpg"
# download_web_image(url)


def initialize():
    periods = 6
    days = 5
    weight_vector = (1,1,1)
    fname = 'input.txt'
    requirements = []
    with open(fname) as f:
        content = f.readlines()
    # you may also want to remove whitespace characters like `\n` at the end of each line
    content = [x.strip() for x in content]
    f.close()
    for line in content:
        #   print(line)
        words = line.split(" ")
       # print(words)
        if words[0] == "Timetable":
            periods = int(words[2])
            days = int(words[6])
            #print(periods, days)
        elif words[0] == 'Schedule':
            cls = int(words[1][1:])
            teacher = int(words[5][1:])
            room = int(words[7][1:])
            times = int(words[8])
           # print(teacher, cls, room, times)
            for i in range(times):
                requirements.append((teacher, cls, room))
        elif words[0] == 'Importance':
            metric = words[2]
            weight = int(words[4])
            if metric == 'Teacherclashes':
                weight_vector = (weight, weight_vector[1],weight_vector[2])
            elif metric == 'Classclashes':
                weight_vector = (weight_vector[0], weight,weight_vector[2])
            elif metric == 'Roomclashes':
                weight_vector = (weight_vector[0], weight_vector[1], weight)

    return periods, days, weight_vector, requirements

periods, days, weight_vector, requirements = initialize()

print(periods, days, weight_vector, len(requirements))
print(requirements)