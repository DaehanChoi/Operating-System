import plotly.figure_factory as ff
from random import randint
from abc import ABC, abstractmethod
# 프로그램이 공유하는 시간 카운터
time = 0
#종료된 프로세스를 저장하는 박스
box = []

class Schedule(ABC):
    @abstractmethod
    def processing(self):
        pass
    def isEmpty(self):
        pass

#프로세스 클래스를 정의

class Process:
    """프로세스의 정보를 보관"""
    def __init__(self, pid, initQue, arrTime, cycle, burstTime):
        self.pid = pid
        self.initQue = initQue
        self.arrTime = arrTime
        self.cycle = cycle
        self.burstTime = burstTime
        self.state = 0  # 현재 상태 0일경우 cpu중, 1일경우 i/o중
        self.end = 0
        self.needTime = sum(burstTime)-cycle+1
    def __str__(self):
        """프로세스 내용물 확인용 메소드"""
        return f"{self.pid} {self.initQue} {self.arrTime} {self.cycle} {self.burstTime} {self.state}"
    
    @classmethod
    def from_string(cls, string_params):
        """프로세스 생성 편의를 위해 string->내용 입력 가능케 해주는 클래스 메소드"""
        temp = string_params.split(" ")
        temp2 = []
        for i in temp:
            temp2.append(int(i))
        temp3=temp2[4:]
        for i in range(len(temp3)):
            if i%2==1:
                temp3[i]+=1
        process = cls(temp2[0], temp2[1], temp2[2], temp2[3], temp3)
        return process


class Rr(Schedule):
    """타임퀀텀을 소모하면 쫒아내는 선점형 방식"""
    def __init__(self, quantum:int):
        self.que = []
        self.quantum = quantum
        self.ab_quantum = quantum

    def processing(self):
        if self.isEmpty():#비어있는 프로세스의 경우 -1리턴 이경우는 모든프로세서가 끝난경우
            return -1
        self.que[0].burstTime[0] -= 1
        self.quantum -= 1

        # 버스트 타임 종료 시
        if self.que[0].burstTime[0] == 0:
            del self.que[0].burstTime[0]
            self.que[0].cycle -= 1
            self.quantum = self.ab_quantum
            #q0과 q1가 같은 RR방식이므로 ab_quantum을통한 구분해야함
            if self.ab_quantum == 2:
                self.que[0].initQue = 0
            else:
                self.que[0].initQue = 1
            return self.que.pop(0)

        # 타임퀀텀 소모시
        if self.quantum == 0:
            self.quantum = self.ab_quantum
            if self.ab_quantum == 2:
                self.que[0].initQue = 1
            else:
                self.que[0].initQue = 2
            return self.que.pop(0)
        return None

    def __str__(self):
        return f"{self.que}"

    def isEmpty(self):
        if len(self.que) == 0:
            return True
        return False


# 그때그때 가장 짧은 것을 계산해서 먼저 처리하는 선점형 방식
class Srtn(Schedule):
    def __init__(self):
        self.que = []
    def processing(self):
        num = self.shortest()
        self.que[num].burstTime[0] -= 1
        # 버스트 타임 종료 시
        if self.que[num].burstTime[0] == 0:
            del self.que[num].burstTime[0]
            self.que[num].cycle -= 1
            self.que[num].initQue = 1
            self.que[num].state = 1
            return self.que.pop(num)
        return None
    def shortest(self):#가장 짧은 프로세스를 선택해주는 함수
        num = 10000
        for i in range(len(self.que)):
            if num > self.que[i].burstTime[0]:
                num = i
        return num
    def __str__(self):
        return f"{self.que}"
    def isEmpty(self):
        if len(self.que) == 0:
            return True
        return False


# 먼저온 프로세서가 먼저 끝나는 비 선점형 방식
class Fcfs(Schedule):
    def __init__(self):
        self.que = []

    def processing(self):
        self.que[0].burstTime[0] -= 1

        # 버스트 타임 종료 시
        if self.que[0].burstTime[0] == 0:
            del self.que[0].burstTime[0]
            self.que[0].cycle -= 1
            self.que[0].initQue = 2
            self.que[0].state = 1
            return self.que.pop(0)

        return None
    def __str__(self):
        return f"{self.que}"
    def isEmpty(self):
        if len(self.que) == 0:
            return True
        return False


class Mfq:
    """Mfq의 역할을 하면서 동시에 프로세스를 실행"""

    def __init__(self):# 네가지 방식의 스케쥴링 큐와 io 중인 프로세스리스트
        self.queZero = Rr(2)
        self.que1 = Rr(6)
        self.que2 = Srtn()
        self.que3 = Fcfs()
        self.io = []
    def addQue(self, processes):#arrTime에 맞춰 프로세스를 넣어준다.
        a = 0
        for i in range(len(processes)):

            if processes[a].arrTime == time:
                if processes[a].initQue == 0:
                    self.queZero.que.append(processes[a])
                    del processes[a]
                elif processes[a].initQue == 1:
                    self.que1.que.append(processes[a])
                    del processes[a]
                elif processes[a].initQue == 2:
                    self.que2.que.append(processes[a])
                    del processes[a]
                else:
                    self.que3.que.append(processes[a])
                    del processes[a]
            else:
                a = a + 1
    def processing(self):
        #모든 큐가 비어있을경우 패스
        if self.queZero.isEmpty() and self.que1.isEmpty() and self.que2.isEmpty() and self.que3.isEmpty():
            pass
        else:#처리할 큐가 있을경우 정책에 맞게 처리
            temp = self.select_que().processing()
            #프로세스가 안끝나고, 타임퀀텀도 남아있는 상황
            if temp == None:
                pass
            elif temp == -1:#다 끝난상황
                pass
            else:
                if temp.state == 1:
                    if len(temp.burstTime) == 0:  # 프로세스 종료시 끝난시간 기록후 박스진입
                        temp.end = time
                        box.append(temp)
                    else:
                        self.io.append(temp)
                else:#쫒겨난 프로세스라면 지정된 위치에 맞게 이동
                    if temp.initQue == 0:
                        self.queZero.que.append(temp)
                    elif temp.initQue == 1:
                        self.que1.que.append(temp)
                    elif temp.initQue == 2:
                        self.que2.que.append(temp)
                    else:
                        self.que3.que.append(temp)
        # io 처리
        a = 0
        if len(self.io) != 0:
            #모든 io한꺼번에 처리
            for line in self.io:
                line.burstTime[0] -= 1
            # io가 0이된 프로세스는 wakeup시켜 다시 큐로보냄    
            for i in range(len(self.io)):

                if self.io[a].burstTime[0] == 0:
                    self.io[a].state = 0
                    del self.io[a].burstTime[0]
                    if self.io[a].initQue == 0:
                        self.queZero.que.append(self.io[a])
                    elif self.io[a].initQue == 1:
                        self.que1.que.append(self.io[a])
                    elif self.io[a].initQue == 2:
                        self.que2.que.append(self.io[a])
                    else:
                        self.que3.que.append(self.io[a])
                    del self.io[a]
                else:
                    a = a + 1
        else:#io가 존재하지 않을경우
            pass
    #큐를 우선순위 순으로 선택해주는 함수
    def select_que(self):
        if self.queZero.isEmpty() == False:
            return self.queZero
        elif self.que1.isEmpty() == False:
            return self.que1
        elif self.que2.isEmpty() == False:
            return self.que2
        elif self.que3.isEmpty() == False:
            return self.que3
        else:#선택할 큐가 없을경우 (큐가 다 비어있을 경우 queZero를 선택한다)
            return self.queZero

    def isEmpty(self):
        if self.queZero.isEmpty() and self.que1.isEmpty() and self.que2.isEmpty() and self.que3.isEmpty() and len(
                self.io) == 0:
            return True
        else:
            return False
        
#시각화를 위해 시간을 시 분 초로 나누어 입력하기 위한 함수
def timeConverter(num):
    hour = num//3600
    minute = (num%3600)//60
    sec = (num%3600)%60
    hour = str(hour).zfill(2)
    minute =str(minute).zfill(2)
    sec = str(sec).zfill(2)
    return hour+":"+minute+":"+sec
fp = open("input.txt", "r")
#진행상황 로그용 큐에서 아무일도 일어나지않으면 0, 일어나면 프로세스의 pid가 기록된다
logQzero = []
logQ1 = []
logQ2 = []
logQ3 = []
num=int(fp.readline())
processes = []  # 프로세스들을 일단 읽어서 전부보관하는 곳
# 프로세스들을 processes에 집어넣음
for line in fp:
    line.strip()
    processes.append(Process.from_string(line))
mfq = Mfq()
# for proc in processes:
#    print(proc)
while 1:
    # 1. processe에 있는 것을 MFQ에 집어넣는다.(arrTime일 경우)
    if len(processes) != 0:
        mfq.addQue(processes)
    # 2. 로그부터 기록한다 각각의 정책에 맞는 우선순위 프로세서가 먼저 실행될 것
    if mfq.queZero.isEmpty() == False:
        logQzero.append(mfq.queZero.que[0].pid)
        logQ1.append(0)
        logQ2.append(0)
        logQ3.append(0)
    elif mfq.que1.isEmpty() == False:
        logQzero.append(0)
        logQ1.append(mfq.que1.que[0].pid)
        logQ2.append(0)
        logQ3.append(0)
    elif mfq.que2.isEmpty() == False:
        logQzero.append(0)
        logQ1.append(0)
        logQ2.append(mfq.que2.que[mfq.que2.shortest()].pid)
        logQ3.append(0)
    elif mfq.que3.isEmpty() == False:
        logQzero.append(0)
        logQ1.append(0)
        logQ2.append(0)
        logQ3.append(mfq.que3.que[0].pid)
    else:
        logQzero.append(0)
        logQ1.append(0)
        logQ2.append(0)
        logQ3.append(0)
    # 3. 1만큼 각자 스케쥴링 정책에 의해 처리 (cpu버스트 소모와 동시에 io도 같이)
    #    끝난 후 타임퀀텀을 초과했다면 다음큐로, 버스트가 완료되었다면 io혹은 완전종료,io를 종료한경우 que에 투입
    mfq.processing()
    # 4.  시간을 1증가시킴
    time = time + 1
    # 5. mfq가 모두 빌때까지 반복
    if mfq.isEmpty() and len(mfq.io) == 0:
        break

##################시각화를 위한 부분##########################
df = []
colors = {}
#인풋의 개수가 변하므로 rgb값을 갯수만큼 만들어준다. 랜덤이긴 하지만 확률이 낮으므로
#색이 겹치진 않을것이라 가정함
for i in range(num):
    mystr = "P"+str(i+1)
    colors[mystr]="rgb({}, {}, {})".format(randint(0,255),randint(0,255),randint(0,255))

# Q0부터Q3까지 차례대로 df 딕셔너리에 집어넣는다.
checker = -1
for i in range(len(logQzero)):
    keyNum = logQzero[i]
    # 빈큐는 무시
    if keyNum == 0:
        continue
    start="2020-04-04 "+timeConverter(i)
    finish="2020-04-04 "+timeConverter(i+1)
    resource="P"+str(keyNum)
    df.append(dict(Task="Q0", Start=start, Finish=finish, Resource=resource))
for i in range(len(logQ1)):
    keyNum = logQ1[i]
    if keyNum == 0:
        continue
    start="2020-04-04 "+timeConverter(i)
    finish="2020-04-04 "+timeConverter(i+1)
    resource="P"+str(keyNum)
    df.append(dict(Task="Q1", Start=start, Finish=finish, Resource=resource))
for i in range(len(logQ2)):
    keyNum = logQ2[i]
    if keyNum == 0:
        continue
    start="2020-04-04 "+timeConverter(i)
    finish="2020-04-04 "+timeConverter(i+1)
    resource="P"+str(keyNum)
    df.append(dict(Task="Q2", Start=start, Finish=finish, Resource=resource))
for i in range(len(logQ3)):
    keyNum = logQ3[i]
    if keyNum == 0:
        continue
    start="2020-04-04 "+timeConverter(i)
    finish="2020-04-04 "+timeConverter(i+1)
    resource="P"+str(keyNum)
    df.append(dict(Task="Q3", Start=start, Finish=finish, Resource=resource))
fig = ff.create_gantt(df, colors=colors, index_col = 'Resource',show_colorbar=True,group_tasks=True)
fig.show()
sum_tt=0
sum_wt=0
for proc in box:
    tt=proc.end-proc.arrTime+1
    wt=tt-proc.needTime
    sum_tt+=tt
    sum_wt+=wt
    print(f"P{proc.pid} : TT = {tt} WT = {wt}")
print(f"AveTT : {sum_tt/num} AveWT : {sum_wt/num}")
fp.close()
