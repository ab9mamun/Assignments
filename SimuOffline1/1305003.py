"""
The task is to simulate an M/M/k system with a single queue. 
Complete the skeleton code and produce results for three experiments.
The study is mainly to show various results of a queue against its ro parameter. 
ro is defined as the ratio of arrival rate vs service rate. 
For the sake of comparison, while plotting results from simulation, also produce the analytical results. 
"""

import heapq
import random
import math
import matplotlib.pyplot as plt
import numpy.random as rand


# Parameters
class Params:
    def __init__(self, lambd, mu, k):        
        self.lambd = lambd
        self.mu = mu
        self.k = k


# States and statistical counters        
class States:
    def __init__(self):
        
        # States
        self.queue = []
        self.busy_count = 0

        self.util_area = 0
        self.queue_area = 0
        self.last_eventTime = 0
        self.total_delay = 0
        
        # Statistics
        self.util = 0.0         
        self.avgQdelay = 0.0
        self.avgQlength = 0.0
        self.served = 0

    def update(self, sim, event):
        None
    
    def finish(self, sim):

        self.util = self.util_area/self.last_eventTime
        self.avgQdelay = self.total_delay/self.served
        self.avgQlength = self.queue_area/self.last_eventTime


        
    def printResults(self, sim):
        # DO NOT CHANGE THESE LINES
        print ('MMk Results: lambda = %lf, mu = %lf, k = %d' %(sim.params.lambd, sim.params.mu, sim.params.k))
        print ('MMk Total customer served: %d' %(self.served)   )
        print ('MMk Average queue length: %lf' %(self.avgQlength))
        print ('MMk Average customer delay in queue: %lf' %(self.avgQdelay)  )
        print ('MMk Time-average server utility: %lf' %(self.util))
        
    def getResults(self, sim):
        return self. avgQlength, self.avgQdelay, self.util
   
class Event:
    def __init__(self, sim):
        self.eventType = None
        self.sim = sim
        self.eventTime = None
        
    def process(self, sim):
        raise Exception('Unimplemented process method for the event!')
    
    def __repr__(self):
        return self.eventType


class StartEvent(Event):
    def __init__(self, eventTime, sim):
        self.eventTime = eventTime
        self.eventType = 'START'
        self.sim = sim
        
    def process(self, sim):
        #None
        A = random.expovariate(sim.params.lambd)
        sim.scheduleEvent(ArrivalEvent(sim.now() +A , sim))
        sim.states.last_eventTime = sim.now()
        sim.scheduleEvent(ExitEvent(1000, sim))


class ExitEvent(Event):    
    def __init__(self, eventTime, sim):
        self.eventTime = eventTime
        self.eventType = 'EXIT'
        self.sim = sim
    
    def process(self, sim):
        sim.states.last_eventTime = sim.now()

                                
class ArrivalEvent(Event):
    def __init__(self, eventTime, sim):
        self.eventType = 'ARRIVAL'
        self.sim = sim
        self.eventTime = eventTime
    def process(self, sim):
        #None
        A = random.expovariate(sim.params.lambd)
        sim.scheduleEvent(ArrivalEvent(sim.now() + A, sim))
        sim.states.queue_area += (sim.now() - sim.states.last_eventTime) * len(sim.states.queue)
        sim.states.util_area += (sim.now() - sim.states.last_eventTime)*(sim.states.busy_count/sim.params.k)
        if sim.states.busy_count < sim.params.k:
            sim.states.busy_count += 1
            D = random.expovariate(sim.params.mu)
            sim.scheduleEvent(DepartureEvent(sim.now() + D, sim))
        else:
            sim.states.queue.append(self.eventTime)
        sim.states.last_eventTime = sim.now()


class DepartureEvent(Event):
    def __init__(self, eventTime, sim):
        self.eventType = 'DEPARTURE'
        self.sim = sim
        self.eventTime = eventTime
    def process(self, sim):
        sim.states.util_area += (sim.now() - sim.states.last_eventTime) * (sim.states.busy_count / sim.params.k)
        sim.states.served+= 1
        if len(sim.states.queue) > 0:

            sim.states.queue_area+= (sim.now() - sim.states.last_eventTime)*len(sim.states.queue)

            D = random.expovariate(sim.params.mu)
            sim.scheduleEvent(DepartureEvent(sim.now() + D, sim))
            now_serving_arrival_time = sim.states.queue.pop()
            sim.states.total_delay+= (sim.now() - now_serving_arrival_time)
        else:
            random.expovariate(0.7)
            sim.states.busy_count -= 1
        sim.states.last_eventTime = sim.now()


class Simulator:
    def __init__(self, seed):
        self.eventQ = []
        self.simclock = 0   
        self.seed = seed
        self.params = None
        self.states = None
        
    def initialize(self):
        self.simclock = 0        
        self.scheduleEvent(StartEvent(0, self))
        
    def configure(self, params, states):
        self.params = params
        self.states = states
            
    def now(self):
        return self.simclock
        
    def scheduleEvent(self, event):
        heapq.heappush(self.eventQ, (event.eventTime, event))        
    
    def run(self):
        random.seed(self.seed)        
        self.initialize()
        
        while len(self.eventQ) > 0:
            time, event = heapq.heappop(self.eventQ)
            
            if event.eventType == 'EXIT':
                break
            
            if self.states != None:
                self.states.update(self, event)
                
            print (event.eventTime, 'Event', event)
            self.simclock = event.eventTime
            event.process(self)
     
        self.states.finish(self)   

    def printResults(self):
        self.states.printResults(self)

    def getResults(self):
        return self.states.getResults(self)
        

def experiment1():
    seed = 101
    sim = Simulator(seed)
    sim.configure(Params(5.0/60, 8.0/60, 1), States())
    sim.run()
    sim.printResults()


def experiment2(k):
    seed = 110
    mu = 1000.0 / 60
    ratios = [u / 10.0 for u in range(1, 11)]

    avglength = []
    avgdelay = []
    util = []
    
    for ro in ratios:
        sim = Simulator(seed)
        sim.configure(Params(mu * ro, mu, k), States())
        sim.run()
        
        length, delay, utl = sim.getResults()
        avglength.append(length)
        avgdelay.append(delay)
        util.append(utl)
        
    plt.figure(1)
    plt.subplot(311)
    plt.plot(ratios, avglength)
    plt.xlabel('Ratio (ro)')
    plt.ylabel('Avg Q length')    

    plt.subplot(312)
    plt.plot(ratios, avgdelay)
    plt.xlabel('Ratio (ro)')
    plt.ylabel('Avg Q delay (sec)')    

    plt.subplot(313)
    plt.plot(ratios, util)
    plt.xlabel('Ratio (ro)')
    plt.ylabel('Util')    
    
    plt.show()			


def experiment3():
    # Similar to experiment2 but for different values of k; 1, 2, 3, 4
    # Generate the same plots
    for k in range(2,5):
        experiment2(k)

def main():
    experiment1()
    #experiment2(1)
    #experiment3()

          
if __name__ == "__main__":
    main()
