#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

// Message-passing library
namespace messaging {

	struct message_base {
		virtual ~message_base(){}
	};

	template<typename Msg>
	struct wrapped_message: public message_base {

		Msg contents;
		explicit wrapped_message(Msg const& contents_) :contents{ contents_ } {}
	};

	struct close_queue{};
	// Message Queue
	class queue {
	public:
		template<typename T>		
		void push(T const& msg) {
			std::lock_guard lock(mtx);
			msg_queue.emplace(std::make_shared<wrapped_message<T>>(msg));
			cond.notify_all();
		}
		std::shared_ptr<message_base> wait_and_pop() {
			std::unique_lock lock(mtx);
			cond.wait(lock,
				[&] { return !msg_queue.empty();}
			);
			auto res = msg_queue.front();
			msg_queue.pop();
			return res;
		}
	private:
		std::mutex mtx;
		std::condition_variable cond;
		std::queue<std::shared_ptr<message_base>> msg_queue;
	};


	template<typename PreviousDispatcher, typename Msg, typename Func>
	class TemplateDispatcher {
	public:
		TemplateDispatcher(TemplateDispatcher const&) = delete;
		TemplateDispatcher& operator=(TemplateDispatcher const&) = delete;

		TemplateDispatcher(TemplateDispatcher&& other)noexcept :
			q(other.q), chained{ other.chained }, f(std::move(other.f)) {			
			other.chained = true;
		}
		TemplateDispatcher(queue* q_, PreviousDispatcher* prev_, Func&& f_)
			:q{ q_ }, prev{ prev_ }, chained{}, f(std::forward<Func>(f_)) {
			prev_->chained = true;
		}
		template<typename OtherMsg,typename OtherFunc>
		TemplateDispatcher<TemplateDispatcher,OtherMsg,OtherFunc>
			handle(OtherFunc&& f) {
			return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(
				q, this, std::forward<OtherFunc>(f));
		}
		~TemplateDispatcher()noexcept(false) {
			if (!chained) {
				wait_and_dispatch();
			}
		}
	private:
		template<typename Dispatcher,typename OtherMsg,typename OtherFunc>
		friend class TemplateDispatcher;
		void wait_and_dispatch() {
			for (;;) {
				auto msg = q->wait_and_pop();
				if (dispatch(msg))
					break;
			}
		}
		bool dispatch(std::shared_ptr<message_base> const& msg) {
			if (wrapped_message<Msg>* wrapper =
				dynamic_cast<wrapped_message<Msg>*>(msg.get())) {
				f(wrapper->contents);
				return true;
			}
			else
			{
				return prev->dispatch(msg);
			}
		}
	private:
		queue* q{};
		PreviousDispatcher *prev{};
		bool chained{};
		Func f;
	};

	
	class dispatcher {
	public:

		dispatcher(dispatcher const&) = delete;
		dispatcher& operator=(dispatcher const&) = delete;

		dispatcher(dispatcher&& other)noexcept
			:q{ other.q }, chained{ other.chained } {
			other.chained = true;
		}

		explicit dispatcher(queue* q_) :q{ q_ }, chained{} {}

		template<typename Message,typename Func>
		TemplateDispatcher<dispatcher,Message,Func>
			handle(Func&& f) {
			return TemplateDispatcher<dispatcher, Message, Func>(q, this, std::forward<Func>(f));
		}
		~dispatcher() noexcept(false){
			if (!chained) {
				wait_and_dispatch();
			}
		}
	private:
		template<typename Dispatcher,
			typename Msg,
			typename Func>
		friend class TemplateDispatcher;
		void wait_and_dispatch() {
			for (;;) {
				auto msg = q->wait_and_pop();
				dispatch(msg);
			}
		}
		bool dispatch(std::shared_ptr<message_base> const& msg) {
			if (dynamic_cast<wrapped_message<close_queue>*>(msg.get())) {
				throw close_queue{};
				return false;
			}
			return true;
		}
	private:
		queue* q;
		bool chained{};
	};



	class sender {
	public:
		sender() :q{} {}

		explicit sender(queue* q_) :q{ q_ } {}

		template<typename Message>
		void send(Message const& msg) {
			if (q) {
				q->push(msg);
			}
		}
	private:
		queue* q;
	};

	class receiver {
	public:
		operator sender() {
			return sender(&q);
		}
		dispatcher wait() {
			return dispatcher(&q);
		}
	private:
		queue q;
	};
}
