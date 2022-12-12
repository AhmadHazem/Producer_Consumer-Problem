#! bin/bash

start:
		@g++ producer.cpp -o producer
		@g++ consumer.cpp -o consumer

clean:
		@rm producer
		@rm consumer
