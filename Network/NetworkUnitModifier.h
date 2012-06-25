#pragma once
#ifndef NETWORKUNITMOD_H
#define NETWORKUNITMOD_H

#include "Network.h"
#include "NetworkUnits.h"

class Unit;

/*class UnitModifier : public NetworkObject
{
public:


protected:

};*/

class UnitModifier : public NetworkObject
{
	#pragma message("Compiling UnitModifier")

public:

	UnitModifier()
	{
	//	m_value = 0;
		m_eventTypeId = 0;
	}

	virtual ~UnitModifier() {}

	long GetFromUnitId()
	{
		return m_fromUnitId;
	}

	void SetFromUnitId(long unitId)
	{
		m_fromUnitId = unitId;
	}

	long GetFromHypercolumnId()
	{
		return m_fromHypercolumnId;
	}

	void SetFromHypercolumnId(long unitId)
	{
		m_fromHypercolumnId = unitId;
	}

	/*float GetValue()
	{
		return m_value;
	}

	void SetValue(float value)
	{
		m_value = value;
	}*/

	short GetEventTypeId()
	{
		return m_eventTypeId;
	}

	std::vector<float> GetEventData()
	{
		return m_eventData;
	}

	// used (?)
	virtual void Send(int nodeId, long unitId){};// = 0;
	virtual void Receive(int nodeId){};// = 0;


	virtual void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit){};// = 0; // will change when additional UnitModifier is added
	virtual void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit){};// = 0; // v2, optimized
	virtual void SimulateV2HypercolumnIds(vector<float> values, vector<float> weights, vector<long> hypercolumnIds, Unit* unit)
	{

	}
	// v3, with delays

	void SetId(int id) { m_id = id; }
	int GetId() { return m_id; }
	void SetUnit(Unit* unit) { m_unit = unit; }

	virtual float GetValue()
	{
		return m_value;
	}

	virtual	void SetValue(float value)
	{
		m_value = value;
	}

	void SetName(string name)
	{
		m_name = name;
	}

	string GetName()
	{
		return m_name;
	}

	virtual void Clear() { };

protected:

	short m_eventTypeId;
	long m_fromUnitId;
	long m_fromHypercolumnId;
	std::vector<float> m_eventData;
	//float m_value;

	// used in transfer functions (may be changed)
	float m_value;
	int m_id;
	string m_name; // used (?)
	Unit* m_unit;
};

class UnitModifierGraded : public UnitModifier
{
public:

	UnitModifierGraded(long fromUnitId, int fromHypercolumnId, float value)
	{
		m_eventTypeId = 1;
		m_fromUnitId = fromUnitId;
		m_fromHypercolumnId = fromHypercolumnId;
		//m_value = value; // not used anymore
		m_eventData.push_back(value);
	}

	void Send(int nodeId, long unitId);
	void Receive(int nodeId);

private:

};




class GeometryUnit : public UnitModifier
{
public:
	GeometryUnit()
	{
		x=y=z=0.0;
		m_id = 7;
		m_name="geometry";
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit)
	{
	}

	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit)
	{
	}

	void SetPosition(float x, float y, float z);

	string GetValuesAsString()
	{
		stringstream ss;
		ss<<x<<" "<<y<<" "<<z;
		return ss.str();
	}

private:
	float x,y,z;
};


class TransferReTIDe : public UnitModifier
{
public:

	TransferReTIDe(Population* layer, float threshold = 1.0, float maxValue = -1, float tau = 20);

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

	void Clear();

private:

#if USE_UNORDERED_MAP == 1
	unordered_map<long,float> m_xt; // slow impl
#else
	map<long,float> m_xt; // slow impl
#endif

	vector<float> m_xtList; // index-based impl
	long m_lowestUnitLocalId;
	int m_nrUnits;

	bool m_firstRun;
	bool m_useHashImpl; // differentiates using a index- or hash-based (in case of large, unordered population) implementation
	float m_threshold, m_tau, m_maxValue;
	long m_localFirstId;
	vector<vector<int> > m_incomingHcIndexes;
};

class TransferThreshold : public UnitModifier
{
public:

	TransferThreshold()
	{
		m_id = 8;
		m_threshold = 0.5;//10;
		m_tau = 30;
		m_firstRun = true;
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

private:

	map<long,float> m_xt; // slow impl
	//vector<float> m_xt; // index-based impl

	bool m_firstRun;
	float m_threshold, m_tau;
	long m_localFirstId;
	vector<vector<int> > m_incomingHcIndexes;
};

// Puts negative values to 0.
class TransferPositive : public UnitModifier
{
public:
	TransferPositive()
	{
		m_id = 10;
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

private:
};

class TransferBcpnnOnline : public UnitModifier
{
public:

	TransferBcpnnOnline(float maxValue = -1)
	{
		m_beta = 0;
		m_id = 2;
		m_name="bcpnn";
		m_maxValue = maxValue;

		m_threshold = 0;
		m_useThreshold = false;
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);
	void SimulateV2HypercolumnIds(vector<float> values, vector<float> weights, vector<long> hypercolumnIds, Unit* unit);

	void SetBeta(float value)
	{
		m_beta = value;
	}
	
	void SetThreshold(float value)
	{
		m_useThreshold = true;
		m_threshold = value;
	}

private:
	float m_threshold;
	bool m_useThreshold;
	float m_beta;
	float m_maxValue;
	vector<vector<int> > m_incomingHcIndexes;
};


class TransferLinear : public UnitModifier
{
public:

	TransferLinear(bool useSign)
	{
		m_useSign = useSign;
		m_useThreshold = false;
		m_id = 1;
	}

	TransferLinear(bool thresholded, float alpha, float eta, float eta3)
	{
		m_useThreshold = thresholded;
		m_alpha = alpha;
		m_useSign = false;
		m_eta3 = eta3;
		m_eta = eta;
		m_id = 1;
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

private:

	// thresholded vars
	bool m_useThreshold;
	float m_eta, m_eta3, m_alpha;
	map<long,float> s; // sensitivity

	bool m_useSign;

	vector<vector<int> > m_incomingHcIndexes;
};

// will output distance from a comparison between weights and input vector
class TransferCSL : public UnitModifier
{
public:

	TransferCSL()
	{
		m_id = 4;
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

	float RRValue(vector<float> x1, vector<float> x2);

private:

	vector<vector<int> > m_incomingHcIndexes;
};

class TransferFoldiak : public UnitModifier
{
public:

	TransferFoldiak(float beta, float alpha, float eta3)
	{
		m_id = 3;
		m_beta = beta;
		m_alpha = alpha;
		m_eta3 = eta3;
	}

	/*void SetParams(float eta3, float alpha)
	{
		m_eta3 = eta3;
		m_alpha = alpha;
	}*/

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

private:

	float m_beta;
	float m_eta3,m_alpha;

	map<long,float> s; // sensitivity

	vector<vector<int> > m_incomingHcIndexes;
};

class TransferTriesch : public UnitModifier
{
public:

	TransferTriesch(float etaIP, float mu_, bool thresholded)
	{
		m_id = 5;
		
		eta_ip = etaIP;//0.005;
		mu = mu_;//0.1; // desired activity
	//	a = b = 0.1;
		m_isThresholded = thresholded;
		m_thresholdLastSpike = false; // impl differently
		m_thresholdLastH = m_thresholdLastY = 0;

		m_value = 0.0;
		m_tau = 0.01f;
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

	void SetDesiredActivity(float m)
	{
		mu = m;
	}

	void Clear();

private:

	bool m_isThresholded;
	bool m_thresholdLastSpike; 
	float m_thresholdLastH;
	float m_thresholdLastY;

	float m_tau;
	map<long,float> a,b;
	float eta_ip, mu;
	vector<vector<int> > m_incomingHcIndexes;
};

class TransferSigmoid : public UnitModifier
{
public:

	TransferSigmoid()
	{
		m_id = 6;
		m_useThreshold = false;
	}

	TransferSigmoid(bool useThreshold)
	{
		m_id = 6;
		m_useThreshold = useThreshold;
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

private:

	bool m_useThreshold;
	vector<vector<int> > m_incomingHcIndexes;
};

class TransferSpikesToRate : public UnitModifier
{
public:

	TransferSpikesToRate()
	{
		m_id = 9;
		m_timeStepsWindow = 1; // instantaneous
	}

	TransferSpikesToRate(int timeStepsWindow)
	{
		m_id = 9;
		m_timeStepsWindow = timeStepsWindow;
	}

	void Simulate(vector<UnitModifier*> events, vector<float> weights, Unit* unit);
	void SimulateV2(vector<float>* values, vector<float>* weights, Unit* unit);

private:

	int m_timeStepsWindow;
	map<long, vector<int> > m_spikesHistory;

};


class UnitModifierSpike : public UnitModifier
{
public:

	UnitModifierSpike(long fromUnitId, int fromHypercolumnId, float time)
	{
		m_eventTypeId = 2;
		m_fromUnitId = fromUnitId;
		m_fromHypercolumnId = fromHypercolumnId;
		//m_time = time; // not used
		m_eventData.push_back(time);
	}

	void Send(int nodeId, long unitId) {}
	void Receive(int nodeId) {}

	/*void SetTime(float time)
	{
		m_time = time;
	}

	float GetTime()
	{
		return m_time;
	}*/

private:

	//float m_time;
};


#endif