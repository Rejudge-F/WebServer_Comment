from locust import HttpLocust, TaskSet, task 

class MyTaskSet(TaskSet):
    @task(2)
    def hello(self):
        self.client.get("/hello.html")
    @task(1)
    def index(self):
        self.client.get("/")

class MyLocust(HttpLocust):
    task_set = MyTaskSet
    min_wait = 0 
    max_wait = 1
