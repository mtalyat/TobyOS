#pragma once

#include "types/types.hpp"

template<typename Signature, uint MaxSubscribers = 16>
class Action;

template<typename... Args, uint MaxSubscribers>
class Action<void(Args...), MaxSubscribers>
{
	public:
	using Handler = void(*)(Args...);

	Action()
		: m_handlers{nullptr}
		, m_count(0)
	{
	}

	bool subscribe(Handler handler)
	{
		if (handler == nullptr || m_count >= MaxSubscribers || contains(handler))
		{
			return false;
		}

		m_handlers[m_count++] = handler;
		return true;
	}

	bool unsubscribe(Handler handler)
	{
		for (uint i = 0; i < m_count; ++i)
		{
			if (m_handlers[i] == handler)
			{
				for (uint j = i + 1; j < m_count; ++j)
				{
					m_handlers[j - 1] = m_handlers[j];
				}

				--m_count;
				m_handlers[m_count] = nullptr;
				return true;
			}
		}

		return false;
	}

    bool operator+=(Handler handler)
    {
        return subscribe(handler);
    }

    bool operator-=(Handler handler)
    {
        return unsubscribe(handler);
    }

	void clear()
	{
		for (uint i = 0; i < m_count; ++i)
		{
			m_handlers[i] = nullptr;
		}

		m_count = 0;
	}

	void invoke(Args... args) const
	{
		for (uint i = 0; i < m_count; ++i)
		{
			m_handlers[i](args...);
		}
	}

	void operator()(Args... args) const
	{
		invoke(args...);
	}

	uint get_size() const
	{
		return m_count;
	}

	static constexpr uint get_capacity()
	{
		return MaxSubscribers;
	}

	private:
	bool contains(Handler handler) const
	{
		for (uint i = 0; i < m_count; ++i)
		{
			if (m_handlers[i] == handler)
			{
				return true;
			}
		}

		return false;
	}

	Handler m_handlers[MaxSubscribers];
	uint m_count;
};