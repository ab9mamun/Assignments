import random

#global variables----------------
periods_per_day = 6
days_per_week = 5
req_length = 3
random_restart_limit = 20
neighbor_attempt_limit = 1000
weight_vector = (1, 1, 10)

num_slots = periods_per_day*days_per_week


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
    if i==j:
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


def random_state(requirements):
    state = []
    for i in range(num_slots):
        state.append([])
    for req in requirements:
        k = random.randrange(num_slots)
        state[k].append(req)

    return state


def hill_climb_stoc(requirements):
    best_state = random_state(requirements)
    best_cost = cost_func(best_state)
    if best_cost == 0:
        return best_state, best_cost
    for iterations in range(random_restart_limit):
        state = random_state(requirements)
        cost = cost_func(state)
        better_neighbor_found = True

        while better_neighbor_found:
            better_neighbor_found = False
            for attempt in range(neighbor_attempt_limit):
                neighbor = find_neighbor(state)
                if neighbor is "failed":
                    continue
                neighbor_cost = cost_func(neighbor)
                if neighbor_cost < cost:
                    state, cost = neighbor, neighbor_cost
                    print(state, cost)
                    better_neighbor_found = True
                    break
        #found a local optima, check for global
        print('Local optima: ',state, cost)
        if cost < best_cost:
            best_state, best_cost = state, cost
        print('Global optima: ', best_state, best_cost)

    return best_state, best_cost


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


current_state = [[(1,2,3),(1,3,5)], [(1,3,7), (2,4,7)]]
#num_slots = 2
requirements = [(1,2,3), (1,3,5), (1,3,7), (2,4,7)]
#print(random_state(requirements))
state, cost = hill_climb_stoc(requirements)
print('Cost value:',cost)
print_state(state)

#temp = [[(1, 3, 5)], [(2, 4, 7), (1, 2, 3), (1, 3, 7)]]
#print(cost_func(temp))


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