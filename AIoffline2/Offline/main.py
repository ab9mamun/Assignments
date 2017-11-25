import random
import time

#function definitions------------------

def cost_func(state):
    cost = 0
    for slot in state:
        length = len(slot)
        for i in range(0, length-1):
            first_req = slot[i]
            for j in range(i+1, length):
                second_req = slot[j]
                for k in range(0, req_length):
                    if first_req[k] == second_req[k]:
                        cost += weight_vector[k]

    return cost


def find_neighbor(cur_state):
    state = cur_state.copy()
    i = random.randrange(0, len(state))
    j = random.randrange(0, len(state))
    if i == j:
        return "failed"

    from_slot = state[i]
    if len(from_slot) <= 1:
        return "failed"
    to_slot = state[j]

    k = random.randrange(0, len(from_slot))
    req = from_slot[k]
    from_slot.pop(k)
    to_slot.append(req)
    return state


def random_state(require_ments):
    state = []
    for i in range(num_slots):
        state.append([])
    for req in require_ments:
        k = random.randrange(num_slots)
        state[k].append(req)

    return state


def hill_climb_stoc(require_ments):
    best_state = random_state(require_ments)
    best_cost = cost_func(best_state)
    if best_cost == 0:
        return best_state, best_cost
    for iterations in range(random_restart_limit):
        print('Random restart: ', iterations+1)
        state = random_state(require_ments)
        better_neighbor_found = True

        while better_neighbor_found:
            better_neighbor_found = False
            for attempt in range(neighbor_attempt_limit):
                neighbor = find_neighbor(state)
                if neighbor == "failed":
                    continue
                neighbor_cost = cost_func(neighbor)
                if neighbor_cost < cost_func(state):
                    state = neighbor.copy()
                    #print(state, cost)
                    better_neighbor_found = True
                    break
        #found a local optima, check for global
        print('Local optima: ', cost_func(state), end=' ')
        if cost_func(state) < cost_func(best_state):
            best_state = state.copy()
            best_cost = cost_func(best_state)
            if best_cost == 0:
                return best_state, 0
        print('Global optima: ', cost_func(best_state))

    return best_state, cost_func(best_state)


def print_state(state):
    print('=============================================')
    for day in range(days_per_week):
        print('Day', day+1,':::')
        for time in range(periods_per_day):
            slot = state[day * periods_per_day + time]
            if len(slot) is 0:
                continue
            print(str(time+9)+':00')
            for req in slot:
                print('Teacher', req[0], 'Class', req[1], 'Room', req[2])
        print('-----------------------')

def initialize():
    periods = 6
    days = 5
    weight__vector = (1,1,1)
    fname = 'input.txt'
    require_ments = []
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
                require_ments.append((teacher, cls, room))
        elif words[0] == 'Importance':
            metric = words[2]
            weight = int(words[4])
            if metric == 'Teacherclashes':
                weight__vector = (weight, weight__vector[1],weight__vector[2])
            elif metric == 'Classclashes':
                weight__vector = (weight__vector[0], weight,weight__vector[2])
            elif metric == 'Roomclashes':
                weight__vector = (weight__vector[0], weight__vector[1], weight)

    return periods, days, weight__vector, require_ments


start_time = time.time()
periods_per_day, days_per_week, weight_vector, requirements = initialize()
#print(periods_per_day, days_per_week, weight_vector)
num_slots = periods_per_day*days_per_week
req_length = 3
random_restart_limit = 400
neighbor_attempt_limit = 100

final_state, final_cost = hill_climb_stoc(requirements)
print('Cost value:', final_cost, cost_func(final_state))
print_state(final_state)
print(final_state)

print('Execution time:',time.time() - start_time, 's')

# temp = [[(2, 4, 3)], [(1, 4, 2), (3, 4, 1), (2, 1, 2), (2, 4, 2), (4, 1, 2), (3, 1, 1), (3, 3, 3), (2, 3, 4), (2, 1, 4)], [(1, 1, 1)], [(3, 3, 1), (1, 4, 2)], [(2, 3, 1), (1, 4, 1)], [(1, 2, 4)], [(4, 1, 2), (4, 3, 1), (1, 1, 4)], [(2, 1, 2), (4, 4, 2), (3, 2, 1)], [(2, 2, 4), (1, 3, 2), (3, 2, 3)], [(3, 2, 3), (1, 3, 1)], [(3, 4, 1), (1, 4, 3), (3, 4, 1)], [(4, 2, 2), (4, 1, 3), (4, 3, 1), (1, 4, 4), (4, 2, 3), (3, 4, 3), (2, 2, 4), (2, 2, 4), (3, 2, 2), (1, 3, 4), (2, 1, 1), (4, 3, 1), (4, 4, 4)], [(2, 1, 3), (2, 1, 2), (4, 2, 4)], [(1, 2, 1), (3, 1, 4), (1, 4, 4)], [(4, 4, 1)], [(3, 2, 2), (1, 3, 4), (1, 1, 4), (1, 1, 4), (4, 2, 4), (3, 3, 3), (3, 2, 3), (3, 2, 3), (2, 2, 1), (1, 3, 3), (4, 2, 1), (2, 1, 2), (3, 1, 4), (3, 3, 3), (4, 4, 1)], [(1, 4, 3), (2, 4, 2), (4, 3, 1), (1, 4, 3), (4, 4, 2)], [(3, 1, 2), (4, 1, 1), (3, 3, 4), (3, 2, 2), (2, 3, 4), (4, 2, 2), (4, 1, 3), (3, 4, 4), (3, 4, 3), (2, 4, 1), (1, 1, 3), (1, 4, 3), (4, 3, 1)], [(4, 2, 4), (3, 1, 3), (4, 2, 4), (3, 4, 2)], [(2, 3, 4), (4, 2, 1), (3, 3, 3), (1, 4, 1)], [(4, 3, 1)], [(2, 2, 2), (2, 2, 4), (1, 3, 4), (2, 3, 3), (4, 3, 4), (2, 3, 2), (2, 3, 2), (1, 1, 2), (2, 4, 1), (1, 3, 3), (4, 4, 3), (2, 2, 2)], [(1, 1, 1), (4, 3, 3), (4, 1, 1)], [(2, 1, 1), (1, 4, 3)], [(3, 3, 4), (2, 1, 2), (1, 4, 3), (2, 2, 2)], [(3, 2, 4)], [(1, 1, 3)], [(2, 2, 2), (1, 1, 2), (3, 3, 2)], [(3, 2, 3)], [(4, 1, 4)]]
# # [[(4, 2, 1), (2, 1, 4), (1, 2, 4), (2, 3, 3), (4, 2, 2)], [(1, 4, 1)], [(1, 3, 3), (2, 4, 1), (3, 3, 4), (4, 4, 2), (1, 4, 3), (1, 4, 4), (4, 2, 4)], [(1, 4, 3), (3, 1, 4)], [(3, 2, 3), (4, 4, 2), (3, 3, 3), (2, 1, 2), (3, 4, 3)], [(3, 1, 2), (3, 4, 1)], [(4, 2, 4), (4, 3, 3), (3, 4, 1), (3, 3, 2), (1, 1, 1)], [(3, 2, 3), (4, 1, 1), (1, 4, 4), (4, 3, 1), (4, 1, 3)], [(2, 1, 2), (4, 2, 4), (4, 1, 2)], [(1, 4, 3), (1, 4, 3), (2, 4, 2), (2, 1, 3), (2, 2, 2), (2, 3, 2)], [(3, 2, 2), (4, 3, 1), (4, 2, 1)], [(2, 1, 2), (1, 4, 1), (4, 3, 4), (2, 3, 4), (1, 3, 4), (3, 4, 4), (3, 4, 2), (4, 3, 1)], [(3, 2, 3), (1, 1, 2)], [(2, 3, 2), (2, 2, 2), (3, 1, 1)], [(2, 2, 4), (2, 2, 1)], [(4, 1, 1)], [(1, 1, 3), (3, 2, 2), (3, 3, 1)], [(3, 3, 4), (3, 3, 3), (1, 1, 2)], [(3, 1, 4), (3, 3, 3)], [(2, 3, 1), (2, 2, 4), (2, 3, 4), (4, 1, 3), (1, 3, 3)], [(2, 4, 2)], [(1, 3, 4), (4, 3, 1), (4, 4, 4), (2, 2, 4), (3, 2, 3), (3, 3, 3), (1, 1, 4), (4, 1, 4), (2, 1, 2), (4, 2, 4), (4, 3, 1), (1, 2, 1), (4, 4, 1)], [(4, 4, 3), (1, 1, 3), (3, 4, 1), (3, 4, 3)], [(3, 2, 3), (1, 1, 4), (2, 1, 2), (2, 3, 4), (3, 2, 2), (4, 1, 2), (4, 3, 1), (4, 4, 1)], [(2, 1, 1), (1, 4, 2), (2, 4, 1), (1, 3, 2)], [(2, 1, 1), (1, 4, 3), (4, 2, 3), (3, 2, 1)], [(1, 3, 1), (3, 2, 4), (2, 2, 4), (4, 2, 2), (1, 4, 3), (3, 1, 3)], [(2, 2, 2), (2, 2, 2)], [(1, 1, 1)], [(1, 4, 2), (1, 3, 4), (1, 1, 4), (2, 4, 3)]]
# print(cost_func(temp))


#Garbage-----------------------------------------------------
# #function definitions------------
# def func():
#     print("in func")
#
#
# func()
# print(slot)
#
# state = [[(1,2,3),(4,5,6)], [(2,5,7), (8,9,11)]]
# print(state[0][1][2])