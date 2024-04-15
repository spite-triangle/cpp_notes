# Event Emitter

>[!tip|style:flat]
> 该文档来自 [utilities-library](https://github.com/wangmengHB/utilities-library/blob/master/packages/util-kit/docs/event.md)

The event-related utils can be found in `util-kit` as below: 

```ts
import { 
	Emitter, PauseableEmitter, AsyncEmitter,
	EventBufferer, EventMultiplexer, 
	Event, 
	CancellationToken, asyncs,
	IWaitUntil,
} from 'util-kit';
```

The Event (Emitter) in vscode has the following features:    
1. [Emitter](#_1-emitter) ( Emitter/PauseableEmitter/AsyncEmitter ) : 
	* [No Event Name Concept](#_11-no-event-name-concept)				
	* [Handling Exception](#_12-handling-exception-in-event-handler)		
	* [In-Order delivery](#_13-in-order-delivery), especially in the scenarior of firing event in handler
	* [main interface in Emitter](#_14-main-interface-in-emitter)
2. [PauseableEmitter](#_2-pausableemitter-extends-emitter) (extends Emitter)	
3. [AsyncEmitter](#_3-asyncemitter-extends-emitter) (extends Emitter)		
4. [EventMultiplexer](#_4-eventmultiplexer) ( an util class for emitter)
5. [EventBufferer](#_5-eventbufferer) ( an util class for emitter)	
6. [Event Utils](#_6-event-utils) ( util functions under the namespace `Event`)
	* [Event.buffer](#_61-eventbuffer)
	* [Event.latch](#_62-eventlatch) 
	* [Event.once](#_63-eventonce) 
	* [Event.stopwatch](#_64-eventstopwatch) 
	* [Event.fromPromise](#_65-eventfrompromise)
	* [Event.debounce](#_66-eventdebounce)

## 1. Emitter
### 1.1. No Event Name Concept
Most of the event emitters are designed like this:
* register handler: `emitter.on('name', handler)`.	
(In vscode, it use the word `event`, functions like `on`, but without event name.)	
* fire event: `emitter.fire('name', data)` or `emitter.trigger('name', data)`.	

If the number of event name grows up, and they are not organized well, it will eventually cause a mess in code, make your project difficult to maintain. 	 
So there is NO event name concept in the event emitter of vscode. It assumes that the event name can be represented by the emitter object itself, intead of the freely defined string. 			  


```ts
import { Event, Emitter, IDisposable } from 'util-kit';

const emitter: Emitter<string> = new Emitter<string>();

function handler(value: string) { ... }

// register event handler
const subscription: IDisposable = emitter.event(handler);

// fire the event
emitter.fire('event data');

// remove and dispose the event handler
subscription.dispose();
```

### 1.2. Handling Exception in event handler
Any exception thrown in event handler will not block the execuation of other event handlers.

```ts
import { Event, Emitter, IDisposable } from 'util-kit';
let emitter = new Emitter<undefined>();
let hit = false;
emitter.event(function () {
    throw new Error('some thing bad!');
});
emitter.event(function () {
    hit = true;
});
emitter.fire(undefined);
console.log('hit', hit);    // hit true
```

### 1.3. In-Order Delivery. 
If any event fire take place in a handler, it will push the fired event in the current delivery queue, and flush it immediately. For example:  

```ts
import { Event, Emitter, IDisposable } from 'util-kit';
const emitter = new Emitter<string>();
const result: string[] = [];

emitter.event(function handler1(val) {
    if (val === 'e1') {
        emitter.fire('e2');
        // a.fire('e2') means:
        // push 'e2' in current delivery queue, and flush it.
        // which means: handler2('e1') -> handler1('e2') -> handler('e2')
        console.log(`result should be ['e1', 'e2']`, result) // ['e1', 'e2']
    }
});

emitter.event(function handler2(val) {
    result.push(val);
});

emitter.fire('e1');
// all events are delivered in order
console.log(`result should be ['e1', 'e2']`, result); // ['e1', 'e2']
```
* Notice: Firing event in any handler should be aware of dead loop, condition statement is MUST.    

### 1.4. main interface in Emitter
If you are using typescript in your project, the vscode IDE intellegence will tell you exactly the details. Here is just a simple and brief introduction.		
* Emitter constructor: (options?: EmitterOptions)

```ts
export interface EmitterOptions {
	onFirstListenerAdd?: Function;
	onFirstListenerDidAdd?: Function;
	onListenerDidAdd?: Function;
	onLastListenerRemove?: Function;
	leakWarningThreshold?: number;
}
```
In most cases, you don't need to pass options into Emitter.		
* emitter.event(handler, context?: any, disposables?: IDisposable[] | DisposableStore)
handler: event handler function.
context: means the `this` context in the event handler.
disposables: An array or disposalStore, it is a collection to store the disposable object used to dispose the event.	

```ts
import { Event, Emitter, IDisposable, DisposableStore } from 'util-kit';

let bucket1: IDisposable[] = [];
const emitter = new Emitter();
const handler = console.log;
let subscription1 = emitter.event(handler, undefined, bucket);

// unhook listener
while (bucket.length) {
	bucket.pop()!.dispose();
}

let bucket2 = new DisposableStore();
let subscription2 = emitter.event(handler, undefined, bucket);
emitter.fire(1);
emitter.fire(2);

// unhook listener
bucket.clear();
```

* emitter.fire(data: any)
The data should be the first parameter in the fire function.


## 2. PausableEmitter (extends Emitter)		
1. pause/resume: If the emitter is paused, it will not really fire any events until it is resumed. If it is paused serveral times, it must be resumed the same times.   	
2. support optional merge function as constructor parameter, used to merge event value during pause phase.
3. if pause ocurs in a listener, it means pause the next event cycle, not the current cycle.

* use case 1

```ts
import { Event, Emitter, IDisposable, PausableEmitter } from 'util-kit';

const data: number[] = [];
const emitter = new PauseableEmitter<number>();

emitter.event(e => data.push(e));
emitter.fire(1);
emitter.fire(2);
console.log('result should be [1, 2]', data);	// [1, 2]

// pause twice here
emitter.pause();
emitter.pause();
emitter.fire(3);
emitter.fire(4);
console.log('result should be [1, 2]', data);	// [1, 2]

// resume once, the emitter is still paused	
emitter.resume();
console.log('result should be [1, 2]', data);	// [1, 2]

// resume twice, the emitter is resumed
emitter.resume();
console.log('result should be [1, 2, 3, 4]', data);	// [1, 2, 3, 4]

// dump resume, nothing happens
emitter.resume();
console.log('result should be [1, 2, 3, 4]', data);	// [1, 2, 3, 4]

emitter.fire(5);
console.log('result should be [1, 2, 3, 4, 5]', data);	// [1, 2, 3, 4, 5]
```
* use case 2: passing the merge function to do sth for the paused events.	 

```ts
import { Event, Emitter, IDisposable, PausableEmitter } from 'util-kit';

const data: number[] = [];
const emitter = new PauseableEmitter<number>({ merge: (a) => a.reduce((p, c) => p + c, 0) });

emitter.event(e => data.push(e));
emitter.fire(1);
emitter.fire(2);
console.log(data);	// [1, 2]

// pause the emitter 
emitter.pause();
emitter.fire(3);
emitter.fire(4);
console.log(data);	// [1, 2]

// resume the emitter, and the event data is merged during the pause phase
emitter.resume();
console.log(data);	// [1, 2, 7]

// behave as normal
emitter.fire(5);
console.log(data);	// [1, 2, 7, 5]

```
* use case 3

```ts
import { PausableEmitter } from 'util-kit';

const data: number[] = [];
const emitter = new PauseableEmitter<number>();

let once = true;
emitter.event(e => {
	data.push(e);
	if (once) {
		// pause the next event cycle, not the current.		
		emitter.pause();
		once = false;
	}
});
emitter.event(e => {
	data.push(e);
});

emitter.pause();
emitter.fire(1);
emitter.fire(2);
console.log('result should be []', data);

emitter.resume();
console.log('result should be [1, 1]', data); 
// paused in the first hanlder, but current cycle will not be blocked

emitter.resume();
console.log('result should be [1, 1, 2, 2]', data); 
// remaining event delivered

emitter.fire(3);
console.log('result should be [1, 1, 2, 2, 3, 3]', data); 
```

## 3. AsyncEmitter (extends Emitter)
1. it has an async `fireAsync` method: 

```ts
async fireAsync(data: Omit<T, 'waitUntil'>, token: CancellationToken, promiseJoin?: (p: Promise<any>, listener: Function) => Promise<any>): Promise<void> {}
```
data: same as normal Emitter.	
token: it is used to control whether pause the current fire. `CancellationToken.None` is used for most cases,  `CancellationToken.Cancelled` is used to pause this fireAsync Action, the event is still in the async delivery queue, which will be executed next time.  
promiseJoin: optional, used to join the waitUntil promise and other custom promise. 		
2. the event data in hanlder has an `waitUntil` function property, it accept a promise as input. The other handler will not be called until this promise is resolved.   
3. Error will NOT block other handlers or can be catched by `fireAsync`, neither error in hanlder or promise in waitUntil. 		 

* use case 1: 

```ts
import { AsyncEmitter, CancellationToken, IWaitUntil, asyncs } from 'util-kit';
const { timeout } = asyncs;

interface E extends IWaitUntil {
	foo: boolean;
}

let result = 0;
let emitter = new AsyncEmitter<E>();
emitter.event(function handler1(e: E) {
	e.waitUntil(timeout(10).then(_ => {
		console.log('result should be 0', result);
		result += 1;
	}));
});

// handler2 is not triggerred until the promise in waitUntil of handler1 is resolved
emitter.event(function handler2(e: E) {
	e.waitUntil(timeout(1).then(_ => {
		console.log('result should be 1', result);
		result += 1;
	}));
});

await emitter.fireAsync({ foo: true }, CancellationToken.None);
console.log('result should be 2', result);

```
* use case 2: 

```ts
import { AsyncEmitter, CancellationToken, IWaitUntil, asyncs } from 'util-kit';
const { timeout } = asyncs;

interface E extends IWaitUntil {
	foo: number;
}
let result: number[] = [];
let done = false;
let emitter = new AsyncEmitter<E>();

// e1
emitter.event(e => {
	e.waitUntil(timeout(10).then(async _ => {
		if (e.foo === 1) {
			// push the event in delivery queue, and flush it
			await emitter.fireAsync({ foo: 2 }, CancellationToken.None);
			console.log('result should be [1, 2]', result);
			done = true;
		}
	}));
});

// e2
emitter.event(e => {
	result.push(e.foo);
	e.waitUntil(timeout(7));
});

await emitter.fireAsync({ foo: 1 }, CancellationToken.None);
console.log('fireAsync is done', done);

```
* use case 3:

```ts
import { AsyncEmitter, CancellationToken, IWaitUntil, asyncs } from 'util-kit';
const { timeout } = asyncs;

interface E extends IWaitUntil {
	foo: boolean;
}

let result = 0;
let emitter = new AsyncEmitter<E>();

emitter.event(e => {
	result += 1;
	e.waitUntil(new Promise((_r, reject) => reject(new Error())));
});

emitter.event(e => {
	result += 1;
	e.waitUntil(timeout(10));
});

await emitter.fireAsync({ foo: true }, CancellationToken.None).then(() => {
	console.log('result should be 2', result);
}).catch(e => {
	// error will never be catched here!
	console.log(e);
});

```


## 4. EventMultiplexer
EventMultiplexer is a helper class for managing multiple emitters. It can make multiple emitter works in one multiplexer. 

```ts
import { Emitter, EventMultiplexer } from 'util-kit';

const result: number[] = [];
const m = new EventMultiplexer<number>();
m.event(r => result.push(r));

const e1 = new Emitter<number>();
m.add(e1.event);
const e2 = new Emitter<number>();
m.add(e2.event);

e1.fire(0);
console.log('result should be [0]', result);

e1.dispose();
console.log('result should be [0]', result);

e1.fire(1);
console.log('result should be [0]', result);

e2.fire(2);
console.log('result should be [0, 2]', result);

const e3 = new Emitter<number>();
const l3 = m.add(e3.event);
e3.fire(3);
console.log('result should be [0, 2, 3]', result);

l3.dispose();
e3.fire(4);
console.log('result should be [0, 2, 3]', result);

e2.fire(4);
e1.fire(5);
console.log('result should be [0, 2, 3, 4]', result);

```

## 5. EventBufferer
The EventBufferer is a helper class for emitter, it is useful in situations in which you want to delay firing your events during some code. 
You can wrap that code and be sure that the event will not be fired during that wrap.

```ts 
import { Emitter, EventBufferer } from 'util-kit';
const emitter: Emitter = new Emitter();;
const delayer = new EventBufferer();
const delayedEvent = delayer.wrapEvent(emitter.event);

let count = 0;
let handler = () => count++;

delayedEvent(handler);
 
delayer.bufferEvents(() => {
	emitter.fire(); // event will not be fired yet
	console.log('count should be 0 ', count);
	emitter.fire(); // event will not be fired yet
	console.log('count should be 0 ', count);
});

// event will only be fired at this point
console.log('count should be 2 ', count);
emitter.fire();
console.log('count should be 3 ', count);

```

## 6. Event Utils 
Event is a namespace, there are some util function in this namespace, mainly used to wrap the `emitter.event` function, to make it has some special features: such as buffer, latch, once, debounce, stopwatch, fromPromise.


### 6.1. Event.buffer

```ts
export function buffer<T>(event: Event<T>, nextTick = false, _buffer: T[] = []): Event<T>
```
Buffers the provided event until a first listener comes along, at which point fire all the events at once and pipe the event from then on.	
* emitter.event: or other event function.			
* isNextTick: whether to fire in the next tick.		
* initData: an array of event data need to be fired sequential as intial fires.

`Event.buffer` only works for the FIRST listener when it is registered.		


```ts
import { Event, Emitter } from 'util-kit';
const result: number[] = [];
const emitter = new Emitter<number>();
// nothing happens.	
emitter.fire(-100);

const bufferedEvent = Event.buffer(emitter.event, false, [-2, -1, 0]);

// the event fired as below will be buffered
emitter.fire(1);
emitter.fire(2);
emitter.fire(3);
console.log('result should be []', result);

// the buffered events will be fired once the first listener registered.
// And only works for the first listener	
const sub1 = bufferedEvent(num => result.push(num));
console.log('result should be [-2, -1, 0, 1, 2, 3]', result);

sub1.dispose();
emitter.fire(4);
// the above fire will not be buffered.
const sub2 = bufferedEvent(num => result.push(num));
// there is no 4 in the result.	
console.log('result should be [-2, -1, 0, 1, 2, 3]', result);
```


### 6.2. Event.latch
The handler will not be triggered if the event data is the same as last time. 


```ts
import { Event, Emitter } from 'util-kit';
const emitter = new Emitter<number>();
const event = Event.latch(emitter.event);

const result: number[] = [];
const sub = event(num => result.push(num));

emitter.fire(1);
console.log('result should be [1]', result);

emitter.fire(2);
console.log('result should be [1, 2]', result);

emitter.fire(2);
// the event data is the same as last time, so nothing happens
console.log('result should be [1, 2]', result);

emitter.fire(1);
console.log('result should be [1, 2, 1]', result);

emitter.fire(1);
// the event data is the same as last time, so nothing happens
console.log('result should be [1, 2, 1]', result);

sub.dispose();
```

### 6.3. Event.once 
The handler will only be triggered once when event fired.

```ts
import { Event, Emitter } from 'util-kit';
const emitter = new Emitter<void>();

let counter1 = 0, counter2 = 0, counter3 = 0;

const listener1 = emitter.event(() => counter1++);
const listener2 = Event.once(emitter.event)(() => counter2++);
const listener3 = Event.once(emitter.event)(() => counter3++);

console.log('counter1 should be 0', counter1);
console.log('counter2 should be 0', counter2);
console.log('counter3 should be 0', counter3);

listener3.dispose();
emitter.fire();
console.log('counter1 should be 1', counter1);
console.log('counter2 should be 1', counter2);
console.log('counter3 should be 0', counter3);

emitter.fire();
console.log('counter1 should be 2', counter1);
console.log('counter2 should be 1', counter2);
console.log('counter3 should be 0', counter3);

listener1.dispose();
listener2.dispose();
```

### 6.4. Event.stopwatch
Given an event, it returns another event which fires only once and as soon as the input event emits. The event data is the number of millis it took for the event to fire.


```ts
const emitter = new Emitter<void>();
const event = Event.stopwatch(emitter.event);

event(duration => {
	console.log(duration);
});

setTimeout(() => emitter.fire(), 10);
```

### 6.5. Event.fromPromise
fromPromise is not a `emitter.event` wrapper, it accept a promise, and return an event function. The listener will be called immediately after the promise resolved. 
It functions like the `process.nextTick`.		


```ts
import { 
	Event, Emitter, asyncs 
} from 'util-kit';
const { timeout } = asyncs;

let count = 0;
const promise = timeout(5);
const event = Event.fromPromise(promise);
// handler will be fired after the promise is resolved.
event(() => count++);

console.log('count should be 0', count);
await promise;
console.log('count should be 1', count);
```

### 6.6. Event.debounce
Debounce the event, the interface is described as below:

```ts
export function debounce<T>(event: Event<T>, merge: (last: T | undefined, event: T) => T, delay?: number, leading?: boolean, leakWarningThreshold?: number): Event<T>;
/**
 * Debounces the provided event, given a `merge` function.
 *
 * @param event The input event.
 * @param merge The reducing function.
 * @param delay The debouncing delay in millis.
 * @param leading Whether the event should fire in the leading phase of the timeout.
 * @param leakWarningThreshold The leak warning threshold override.
 */
```
Use Case 1:

```ts
import { Event, Emitter } from 'util-kit';

const emitter = new Emitter<string>();
const debouncedEvent = Event.debounce(emitter.event, (pre, cur) => cur, 10);

const handler1 = (val: string) => { console.log('debounce event fired:', val); };
const sub1 = debouncedEvent(handler1);

emitter.fire('1');
emitter.fire('2');
emitter.fire('3');
// debounce event fired: 3

```

Use Case 2:	

```ts
import { Event, Emitter } from 'util-kit';

const emitter = new Emitter<string>();
const debouncedEvent = Event.debounce(emitter.event, (pre, cur) => cur, 10);

const handler1 = (val: string) => { console.log('debounce event fired:', val); };
const sub1 = debouncedEvent(handler1);

emitter.fire('1');
emitter.fire('2');
emitter.fire('3');

sub1.dispose();
// handler1 will never be triggered, 
// because debounced event would be execuated in next macro task. 
// It is already disposed at that moment.

// leading: true
const debouncedEventLeading = Event.debounce(emitter.event, (pre, cur) => cur, 10, true);

const handler2 = (val: string) => { console.log('debounce event fired while leading true:', val)};
const sub2 = debouncedEventLeading(handler2);

emitter.fire('1');
emitter.fire('2');
emitter.fire('3');

sub2.dispose();
// debounce event fired while leading true: 1
```

Use Case 3:

```ts
import { Event, Emitter } from 'util-kit';

const emitter = new Emitter<number>();
const debouncedEvent = Event.debounce(emitter.event, (pre, cur) => pre? pre + cur: cur, 10, true);

const handler1 = (val: number) => { console.log('debounce event fired:', val); };
const sub1 = debouncedEvent(handler1);

emitter.fire(1);
emitter.fire(2);
emitter.fire(3);

// debounce event fired: 1
// debounce event fired: 5 
// 5: 2 + 3 in the merge function: (pre, cur) => pre? pre + cur: cur 
```