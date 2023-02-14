/*
 * @Author: yangzijian
 * @Date: 2021-09-18 15:42:36
 * @LastEditTime: 2021-09-18 16:00:36
 * @LastEditors: yangzijian
 * @Description: eventManager 
 * @FilePath: \Project1\EventManager.h
 */
#pragma once

#include "EventCode.h"
#include <vector>
#include <map>
#include <functional>

namespace Common
{
	class EventArgBase
	{
	};
	template <class Type>
	class EATemplate : public EventArgBase
	{
	public:
		EATemplate(const Type &v) : value(v) {}
		const Type &value;
	};

	class EventArg
	{
	public:
		using CallBackArgType = std::vector<EventArgBase *>;
		~EventArg()
		{
			for (auto ptr : m_arrArg)
			{
				delete ptr;
			}
			m_arrArg.clear();
		}
		CallBackArgType m_arrArg;
		template <typename T>
		void AddArg(const T &arg)
		{
			m_arrArg.push_back(new EATemplate<T>(arg));
		}
		template <typename T>
		const T &GetArg(int nIndex) const
		{
			const EATemplate<T> *_value = static_cast<const EATemplate<T> *>(m_arrArg[nIndex]);
			return _value->value;
		}
	};

	class EventManager
	{
	public:
		typedef const EventArg* Function_Arg_Type_const;
		typedef std::function<void(Function_Arg_Type_const)> Function_Type;
		//using Function_Arg_Type_const = typename const EventArg*;
		//using Function_Type = typename std::function<void(Function_Arg_Type_const)>;

	protected:
		typedef EventArg* Function_Arg_Type;
		typedef std::map<unsigned int, Function_Type> IDFucntion_Map;
		typedef std::map<EventCode, IDFucntion_Map> EventCode_Map;

		//using Function_Arg_Type = typename EventArg *;
		//using IDFucntion_Map = typename std::map<unsigned int, Function_Type>;
		//using EventCode_Map = typename std::map<EventCode, IDFucntion_Map>;

	public:
		EventManager()
		{
		}
		virtual ~EventManager()
		{
		}
		EventManager(const EventManager &) = delete;
		EventManager &operator=(const EventManager &) = delete;

		unsigned int Register(EventCode eCode, Function_Type pCallBack)
		{
			unsigned int id = IDCreater();
			IDFucntion_Map &mpCallback = m_mpEvent[eCode];
			mpCallback.insert(std::make_pair(id, pCallBack));
			return id;
		}
		void UnRegister(unsigned int nRegisterCode)
		{
			for (auto eventIt : m_mpEvent)
			{
				for (auto funcIt : eventIt.second)
				{
					if (funcIt.first == nRegisterCode)
					{
						eventIt.second.erase(funcIt.first);
						return;
					}
				}
			}
		}

		void UnRegister(EventCode eCode, unsigned int nRegisterCode)
		{
			auto fIt = m_mpEvent.find(eCode);
			if (fIt != m_mpEvent.end())
			{
				fIt->second.erase(nRegisterCode);
			}
		}

		void Notify(EventCode eCode, Function_Arg_Type_const arg = nullptr)
		{
			auto fIt = m_mpEvent.find(eCode);
			if (fIt != m_mpEvent.end())
			{
				for (auto pFuncIt : fIt->second)
				{
					pFuncIt.second(arg);
				}
			}
		}

		void Notify(EventCode eCode, const EventArg &arg)
		{
			Notify(eCode, (EventManager::Function_Arg_Type_const)&arg.m_arrArg);
		}

	private:
		unsigned int IDCreater()
		{
			return m_nIdCounter++;
		}

	private:
		EventCode_Map m_mpEvent;
		unsigned int m_nIdCounter = 0;
	};

	class EventHelper
	{
	public:
		EventHelper(EventManager &tEventManager) : m_tEventManager(tEventManager)
		{
		}
		EventHelper(const EventHelper &) = delete;
		EventHelper &operator=(const EventHelper &) = delete;
		~EventHelper()
		{
			UnRegisterAll();
		}

		unsigned int Register(EventCode eCode, EventManager::Function_Type pCallBack)
		{
			unsigned int id = m_tEventManager.Register(eCode, pCallBack);
			m_mpEventList[eCode].push_back(id);
			return id;
		}

		void UnRegister(EventCode eCode)
		{
			auto fIt = m_mpEventList.find(eCode);
			if (fIt != m_mpEventList.end())
			{
				for (auto code : fIt->second)
				{
					m_tEventManager.UnRegister(eCode, code);
				}
				fIt->second.clear();
			}
		}

		void UnRegisterAll()
		{
			for (auto it : m_mpEventList)
			{
				for (auto code : it.second)
				{
					m_tEventManager.UnRegister(it.first, code);
				}
			}
			m_mpEventList.clear();
		}

	public:
		EventManager &m_tEventManager;

	private:
		std::map<EventCode, std::vector<unsigned int>> m_mpEventList;
	};
}
